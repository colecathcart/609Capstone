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

using namespace std;

// Type definitions for WebSocket++ server and client
typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::client<websocketpp::config::asio> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

// Global variables
server ws_server;
client ws_client;
websocketpp::connection_hdl client_hdl;
bool is_client_mode = false;
bool connected = false;
sqlite3* db = nullptr;
constexpr int WEBSOCKET_PORT = 9002;

// Track connected clients (for host mode)
set<websocketpp::connection_hdl, owner_less<websocketpp::connection_hdl>> connections;
map<websocketpp::connection_hdl, string, owner_less<websocketpp::connection_hdl>> device_names;

// Utility: Get hostname (used as device name)
string get_device_name() {
    char hostname[128];
    gethostname(hostname, sizeof(hostname));
    return string(hostname);
}

// Send message to all connected clients (only host does this)
void broadcast(const string& message) {
    for (auto& hdl : connections) {
        try {
            ws_server.send(hdl, message, websocketpp::frame::opcode::text);
        } catch (...) {
            // Silently ignore send failures
        }
    }
}

// Initialize SQLite DB and tables if needed
void init_db() {
    if (sqlite3_open("data/stats.db", &db)) {
        cerr << "Failed to open SQLite DB\n";
        exit(1);
    }

    const char* create_stats = R"(
        CREATE TABLE IF NOT EXISTS system_stats (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            suspicious_detected INTEGER DEFAULT 0,
            processes_killed INTEGER DEFAULT 0,
            total_devices INTEGER DEFAULT 6,
            compromises INTEGER GENERATED ALWAYS AS (suspicious_detected - processes_killed) STORED
        );
        INSERT OR IGNORE INTO system_stats (id, suspicious_detected, processes_killed, total_devices)
        VALUES (1, 0, 0, 6);
    )";

    const char* create_devices = R"(
        CREATE TABLE IF NOT EXISTS devices (
            name TEXT PRIMARY KEY,
            status TEXT DEFAULT 'Offline'
        );
    )";

    sqlite3_exec(db, create_stats, nullptr, nullptr, nullptr);
    sqlite3_exec(db, create_devices, nullptr, nullptr, nullptr);
}

// Fetch combined stats + device status as JSON string
string get_combined_payload() {
    sqlite3_stmt* stmt;
    string stats_json = "{";
    string query_stats = "SELECT suspicious_detected, processes_killed, total_devices, compromises FROM system_stats WHERE id=1";

    if (sqlite3_prepare_v2(db, query_stats.c_str(), -1, &stmt, nullptr) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        stats_json += "\"suspiciousDetected\": " + to_string(sqlite3_column_int(stmt, 0)) + ",";
        stats_json += "\"processesKilled\": " + to_string(sqlite3_column_int(stmt, 1)) + ",";
        stats_json += "\"totalDevices\": " + to_string(sqlite3_column_int(stmt, 2)) + ",";
        stats_json += "\"compromises\": " + to_string(sqlite3_column_int(stmt, 3));
    }
    sqlite3_finalize(stmt);
    stats_json += "}";

    // Devices info
    string devices_json = "[";
    string query_devices = "SELECT name, status FROM devices";
    bool first = true;

    if (sqlite3_prepare_v2(db, query_devices.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) devices_json += ",";
            devices_json += "{";
            devices_json += "\"name\":\"" + string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) + "\",";
            devices_json += "\"status\":\"" + string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + "\"";
            devices_json += "}";
            first = false;
        }
    }
    sqlite3_finalize(stmt);
    devices_json += "]";

    return "{ \"stats\": " + stats_json + ", \"devices\": " + devices_json + " }";
}

// Insert or update device status
void set_device_status(const string& name, const string& status) {
    string sql = "INSERT INTO devices (name, status) VALUES (?, ?) ON CONFLICT(name) DO UPDATE SET status=excluded.status;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, status.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    broadcast(get_combined_payload());
}

// Increment a field in system_stats
void increment_stat(const string& field) {
    string sql = "UPDATE system_stats SET " + field + " = " + field + " + 1 WHERE id=1;";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

// Shared entry point: called by Analyzer when suspicious/killed
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

// HOST: Start WebSocket++ server and listen for clients
void start_websocket_server() {
    is_client_mode = false;
    init_db();

    ws_server.set_open_handler([](websocketpp::connection_hdl hdl) {
        string name = get_device_name();
        connections.insert(hdl);
        device_names[hdl] = name;
        set_device_status(name, "Online");
        ws_server.send(hdl, get_combined_payload(), websocketpp::frame::opcode::text);
    });

    ws_server.set_close_handler([](websocketpp::connection_hdl hdl) {
        string name = device_names[hdl];
        set_device_status(name, "Offline");
        connections.erase(hdl);
        device_names.erase(hdl);
    });

    ws_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        string type = msg->get_payload();
        string name = device_names[hdl];

        if (type == "SUSPICIOUS") {
            increment_stat("suspicious_detected");
            set_device_status(name, "Threat Detected");
        } else if (type == "KILLED") {
            increment_stat("processes_killed");
        }

        broadcast(get_combined_payload());
    });

    ws_server.init_asio();
    ws_server.set_reuse_addr(true);
    ws_server.listen(WEBSOCKET_PORT);
    ws_server.start_accept();

    cout << "[WebSocket] Host running on port 9002\n";
    ws_server.run();
    sqlite3_close(db);
}

// CLIENT: Connect to host and send updates
void connect_to_websocket_host(const string& uri) {
    is_client_mode = true;

    ws_client.init_asio();
    ws_client.set_open_handler([&](websocketpp::connection_hdl hdl) {
        client_hdl = hdl;
        connected = true;
        cout << "[WebSocket] Connected to host.\n";
    });

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
