#include "websocket_server.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <sqlite3.h>
#include <unistd.h>
#include <iostream>
#include <set>
#include <map>
#include <string>

using namespace std;

// WebSocket++ typedefs for convenience
typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::client<websocketpp::config::asio> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// Global instances and flags
server ws_server;               // WebSocket server instance
client ws_client;               // WebSocket client instance
websocketpp::connection_hdl client_hdl;  // Connection handle when in client mode
bool is_client_mode = false;    // Indicates client (true) or server (false) mode
bool connected = false;         // Tracks if client is connected
sqlite3* db = nullptr;          // Global pointer to SQLite database
constexpr int WEBSOCKET_PORT = 9002; // Port for the WebSocket server

// In server mode, track connections and map them to device names
set<websocketpp::connection_hdl, owner_less<websocketpp::connection_hdl>> connections;
map<websocketpp::connection_hdl, string, owner_less<websocketpp::connection_hdl>> device_names;

// Returns the system's hostname as a string
string get_device_name() {
    char hostname[128];
    gethostname(hostname, sizeof(hostname));
    return string(hostname);
}

// Sends a text message to all connections currently tracked
void broadcast(const string& message) {
    for (auto& hdl : connections) {
        try {
            ws_server.send(hdl, message, websocketpp::frame::opcode::text);
        } catch (...) {
            // Ignore any send errors
        }
    }
}

// Opens or creates the database file and sets up necessary tables
void init_db() {
    if (sqlite3_open("data/stats.db", &db)) {
        cerr << "Failed to open SQLite DB\n";
        exit(1);
    }

    // Create or verify system_stats table with an initial record
    const char* create_stats = R"(
        CREATE TABLE IF NOT EXISTS system_stats (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            suspicious_detected INTEGER DEFAULT 0,
            processes_killed INTEGER DEFAULT 0,
            total_devices INTEGER DEFAULT 2,
            compromises INTEGER GENERATED ALWAYS AS (suspicious_detected - processes_killed) STORED
        );
        INSERT OR IGNORE INTO system_stats (id, suspicious_detected, processes_killed, total_devices)
        VALUES (1, 0, 0, 2);
    )";

    // Create or verify devices table
    const char* create_devices = R"(
        CREATE TABLE IF NOT EXISTS devices (
            name TEXT PRIMARY KEY,
            status TEXT DEFAULT 'Offline'
        );
    )";

    // Create or verify blacklist_hashes table
    const char* create_blacklist = R"(
        CREATE TABLE IF NOT EXISTS blacklist_hashes (
            hash TEXT PRIMARY KEY,
            last_seen TEXT DEFAULT (datetime('now'))
        );
    )";

    sqlite3_exec(db, create_stats,    nullptr, nullptr, nullptr);
    sqlite3_exec(db, create_devices,  nullptr, nullptr, nullptr);
    sqlite3_exec(db, create_blacklist,nullptr, nullptr, nullptr);
}

// Builds a JSON string containing stats, devices, and blacklisted hashes
string get_combined_payload() {
    static const char* Q_STATS   = "SELECT suspicious_detected, processes_killed, total_devices, compromises FROM system_stats WHERE id=1";
    static const char* Q_DEVICES = "SELECT name, status FROM devices";
    static const char* Q_BLACK   = "SELECT hash, last_seen FROM blacklist_hashes";

    sqlite3_stmt* stmt = nullptr;

    // Retrieve system stats
    string stats_json = "{}";
    if (sqlite3_prepare_v2(db, Q_STATS, -1, &stmt, nullptr) == SQLITE_OK &&
        sqlite3_step(stmt) == SQLITE_ROW)
    {
        stats_json = "{"
            "\"suspiciousDetected\":" + to_string(sqlite3_column_int(stmt, 0)) + ","
            "\"processesKilled\":"    + to_string(sqlite3_column_int(stmt, 1)) + ","
            "\"totalDevices\":"       + to_string(sqlite3_column_int(stmt, 2)) + ","
            "\"compromises\":"        + to_string(sqlite3_column_int(stmt, 3)) +
        "}";
    }
    sqlite3_finalize(stmt);

    // Retrieve devices
    string devices_json = "[";
    bool first_device   = true;
    if (sqlite3_prepare_v2(db, Q_DEVICES, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first_device) devices_json += ",";
            devices_json += "{"
                "\"name\":\""   + string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) + "\","
                "\"status\":\"" + string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + "\""
            "}";
            first_device = false;
        }
    }
    sqlite3_finalize(stmt);
    devices_json += "]";

    // Retrieve blacklisted hashes
    string blacklist_json = "[";
    bool first_hash       = true;
    if (sqlite3_prepare_v2(db, Q_BLACK, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first_hash) blacklist_json += ",";

            // Column 0 = hash, Column 1 = last_seen
            string hash     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string lastSeen = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            blacklist_json += "{"
                "\"hash\":\""     + hash     + "\","
                "\"lastSeen\":\"" + lastSeen + "\""
            "}";
            first_hash = false;
        }
    }
    sqlite3_finalize(stmt);
    blacklist_json += "]";

    // Combine into a single JSON payload
    return "{"
        "\"stats\":"       + stats_json     + ","
        "\"devices\":"     + devices_json   + ","
        "\"blacklisted\":" + blacklist_json +
    "}";
}

// Inserts or updates a device's status
void set_device_status(const string& name, const string& status) {
    string sql = R"(
        INSERT INTO devices (name, status) VALUES (?, ?)
        ON CONFLICT(name) DO UPDATE SET status = excluded.status
    )";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, status.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

// Increments a specific field in the system_stats table
void increment_stat(const string& field) {
    string sql = "UPDATE system_stats SET " + field + " = " + field + " + 1 WHERE id=1;";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

// Registers a device name for the given connection if new, sets online status
void register_new_device(websocketpp::connection_hdl hdl, const string& name) {
    connections.insert(hdl);
    device_names[hdl] = name;

    // Check if this device is new in the database
    sqlite3_stmt* stmt;
    string check_sql = "SELECT COUNT(*) FROM devices WHERE name = ?";
    bool is_new_device = false;

    if (sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 0) {
            is_new_device = true;
        }
        sqlite3_finalize(stmt);
    }

    // Set online status for this device
    set_device_status(name, "Online");
    // If brand-new, increment total_devices
    if (is_new_device) {
        increment_stat("total_devices");
    }
}

// Called by an Analyzer to update suspicious/killed counts. 
// If client mode, sends to server; otherwise updates DB locally.
void send_stat_update(const string& type) {
    if (is_client_mode && connected) {
        ws_client.send(client_hdl, type, websocketpp::frame::opcode::text);
    } else {
        if (type == "SUSPICIOUS") {
            increment_stat("suspicious_detected");
            set_device_status(get_device_name(), "Threat Detected");
        } else if (type == "KILLED") {
            increment_stat("processes_killed");
        }
        broadcast(get_combined_payload());
    }
}

// Adds a hash to the blacklist table. If in client mode, sends command to server.
void add_blacklisted_hash(const string& hash) {
    if (is_client_mode && connected) {
        // Client: send command to the host
        string newHash = "ADD_HASH:" + hash;
        ws_client.send(client_hdl, newHash, websocketpp::frame::opcode::text);
    } else {
        // Host: upsert with last_seen = current time
        static const char* INSERT_HASH = R"(
            INSERT INTO blacklist_hashes (hash, last_seen)
            VALUES (?, datetime('now'))
            ON CONFLICT(hash) DO UPDATE SET last_seen = datetime('now')
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, INSERT_HASH, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, hash.c_str(), -1, SQLITE_STATIC);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }
}

// Starts the WebSocket server in host mode, listens for connections
void start_websocket_server() {
    is_client_mode = false;
    init_db();

    // Called when a new connection is opened
    ws_server.set_open_handler([](websocketpp::connection_hdl hdl) {
        string name = get_device_name();
        connections.insert(hdl);
        // If no device name is recorded yet, register it
        if (device_names.find(hdl) == device_names.end()) {
            register_new_device(hdl, name);
        }
        // Send an initial snapshot of data
        ws_server.send(hdl, get_combined_payload(), websocketpp::frame::opcode::text);
    });

    // Called when a connection closes
    ws_server.set_close_handler([](websocketpp::connection_hdl hdl) {
        string name = device_names[hdl];
        if (!name.empty()) {
            set_device_status(name, "Offline");
        }
        connections.erase(hdl);
        device_names.erase(hdl);
    });

    // Called when a message is received
    ws_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        string payload = msg->get_payload();

        // If this connection isn't registered, treat payload as hostname
        if (device_names.find(hdl) == device_names.end()) {
            register_new_device(hdl, payload);
            return;
        }

        bool recognized = false;

        // Check for known commands
        if (payload == "SUSPICIOUS") {
            increment_stat("suspicious_detected");
            set_device_status(device_names[hdl], "Threat Detected");
            recognized = true;
        } else if (payload == "KILLED") {
            increment_stat("processes_killed");
            recognized = true;
        } else {
            // Check for ADD_HASH command
            const string prefix = "ADD_HASH:";
            if (payload.rfind(prefix, 0) == 0) {
                string hashValue = payload.substr(prefix.size());
                add_blacklisted_hash(hashValue);
                recognized = true;
            }
        }

        // Broadcast updates if a known command was processed
        if (recognized) {
            broadcast(get_combined_payload());
        } else {
            cerr << "[WebSocket] Unhandled command: " << payload << endl;
        }
    });

    ws_server.init_asio();
    ws_server.set_reuse_addr(true);
    ws_server.listen(WEBSOCKET_PORT);
    ws_server.start_accept();

    cout << "[WebSocket] Host running on port " << WEBSOCKET_PORT << "\n";
    ws_server.run();

    sqlite3_close(db);
}

// Connects to a remote server in client mode
void connect_to_websocket_host(const string& uri) {
    is_client_mode = true;

    ws_client.set_access_channels(websocketpp::log::alevel::all);
    ws_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

    ws_client.init_asio();
    ws_client.set_open_handler([&](websocketpp::connection_hdl hdl) {
        client_hdl = hdl;
        connected = true;
        cout << "[WebSocket] Connected to host.\n";
        // Send local hostname to register with the server
        ws_client.send(hdl, get_device_name(), websocketpp::frame::opcode::text);
    });

    // Logs incoming messages on the client side
    ws_client.set_message_handler([](websocketpp::connection_hdl, message_ptr msg) {
        cout << "[WebSocket] Received: " << msg->get_payload() << endl;
    });

    websocketpp::lib::error_code ec;
    auto con = ws_client.get_connection(uri, ec);
    if (ec) {
        cerr << "[WebSocket] Connection failed: " << ec.message() << endl;
        return;
    }

    ws_client.connect(con);
    ws_client.run();
}
