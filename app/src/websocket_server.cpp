#include "websocket_server.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <sqlite3.h>
#include <iostream>
#include <set>
#include <map>
#include <unistd.h>  // for gethostname

typedef websocketpp::server<websocketpp::config::asio> server;

server ws_server;
sqlite3* db;

// Track active WebSocket clients and their hostnames
set<websocketpp::connection_hdl, owner_less<websocketpp::connection_hdl>> connections;
map<websocketpp::connection_hdl, string, owner_less<websocketpp::connection_hdl>> device_names;

// Broadcast message to all connected clients
void broadcast(const string& message) {
    for (auto& hdl : connections) {
        try {
            ws_server.send(hdl, message, websocketpp::frame::opcode::text);
        } catch (...) {
            // Fail silently
        }
    }
}

// Initialize SQLite DB and default stats row
void init_db() {
    if (sqlite3_open("data/stats.db", &db)) {
        cerr << "Failed to open SQLite DB\n";
        exit(1);
    }

    const char* create_system_stats_sql = R"(
        CREATE TABLE IF NOT EXISTS system_stats (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            suspicious_detected INTEGER DEFAULT 0,
            processes_killed INTEGER DEFAULT 0,
            total_devices INTEGER DEFAULT 6,
            compromises INTEGER DEFAULT 0
        );
        INSERT OR IGNORE INTO system_stats (id) VALUES (1);
    )";
    sqlite3_exec(db, create_system_stats_sql, nullptr, nullptr, nullptr);

    const char* create_devices_sql = R"(
        CREATE TABLE IF NOT EXISTS devices (
            name TEXT PRIMARY KEY,
            status TEXT DEFAULT 'Offline'
        );
    )";
    sqlite3_exec(db, create_devices_sql, nullptr, nullptr, nullptr);
}

// Combined payload: system stats + device statuses
string get_combined_payload() {
    // 1. System stats
    sqlite3_stmt* stmt;
    string stats_json = "{";
    string stats_query = "SELECT suspicious_detected, processes_killed, total_devices, compromises FROM system_stats WHERE id=1";

    if (sqlite3_prepare_v2(db, stats_query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats_json += "\"suspiciousDetected\": " + to_string(sqlite3_column_int(stmt, 0)) + ",";
            stats_json += "\"processesKilled\": " + to_string(sqlite3_column_int(stmt, 1)) + ",";
            stats_json += "\"totalDevices\": " + to_string(sqlite3_column_int(stmt, 2)) + ",";
            stats_json += "\"compromises\": " + to_string(sqlite3_column_int(stmt, 3));
        }
        sqlite3_finalize(stmt);
    }
    stats_json += "}";

    // 2. Device statuses
    string devices_json = "[";
    string query = "SELECT name, status FROM devices";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        bool first = true;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) devices_json += ",";
            devices_json += "{";
            devices_json += "\"name\":\"" + string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) + "\",";
            devices_json += "\"status\":\"" + string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + "\"";
            devices_json += "}";
            first = false;
        }
        sqlite3_finalize(stmt);
    }
    devices_json += "]";

    // 3. Final payload
    return "{ \"stats\": " + stats_json + ", \"devices\": " + devices_json + " }";
}

// Update or insert device status
void set_device_status(const string& name, const string& status) {
    string sql = "INSERT INTO devices (name, status) VALUES (?, ?) "
                 "ON CONFLICT(name) DO UPDATE SET status=excluded.status;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, status.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    broadcast(bined_payload());
}

// Stat increment helper
void increment_stat(const string& field) {
    string sql = "UPDATE system_stats SET " + field + " = " + field + " + 1 WHERE id=1;";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

// Analyzer entry points
void increment_suspicious() {
    increment_stat("suspicious_detected");
    broadcast(get_combined_payload());
}

void increment_killed() {
    increment_stat("processes_killed");
    broadcast(get_combined_payload());
}

// Start the server
void start_websocket_server() {
    init_db();

    ws_server.set_open_handler([](websocketpp::connection_hdl hdl) {
        char hostname[128];
        gethostname(hostname, sizeof(hostname));
        string name = hostname;

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
        cout << "Message received: " << msg->get_payload() << endl;
    });

    ws_server.init_asio();
    ws_server.listen(9002);
    ws_server.start_accept();

    cout << "WebSocket++ server running on port 9002\n";
    ws_server.run();

    sqlite3_close(db);
}
