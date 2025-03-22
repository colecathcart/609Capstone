#include "../include/websocket_server.h"
#include "../third_party/websocketpp/websocketpp/config/asio_no_tls.hpp"
#include "../third_party/websocketpp/websocketpp/server.hpp"
#include <sqlite3.h>
#include <iostream>
#include <set>

typedef websocketpp::server<websocketpp::config::asio> server;

server ws_server;
sqlite3* db;

// Store active connections
std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections;

// Broadcast JSON stats to all connected WebSocket clients
void broadcast(const std::string& message) {
    for (auto& hdl : connections) {
        try {
            ws_server.send(hdl, message, websocketpp::frame::opcode::text);
        } catch (...) {
            // Ignore send errors silently
        }
    }
}

// Initialize SQLite DB and default stats row
void init_db() {
    if (sqlite3_open("app/data/stats.db", &db)) {
        std::cerr << "Failed to open SQLite DB\n";
        exit(1);
    }

    const char* create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS system_stats (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            suspicious_detected INTEGER DEFAULT 0,
            processes_killed INTEGER DEFAULT 0,
            total_devices INTEGER DEFAULT 6,
            compromises INTEGER DEFAULT 0
        );
        INSERT OR IGNORE INTO system_stats (id) VALUES (1);
    )";

    sqlite3_exec(db, create_table_sql, nullptr, nullptr, nullptr);
}

// Return all stats as a JSON string
std::string get_stats() {
    sqlite3_stmt* stmt;
    std::string query = "SELECT suspicious_detected, processes_killed, total_devices, compromises FROM system_stats WHERE id=1";
    std::string json = "{";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            json += "\"suspiciousDetected\": " + std::to_string(sqlite3_column_int(stmt, 0)) + ",";
            json += "\"processesKilled\": " + std::to_string(sqlite3_column_int(stmt, 1)) + ",";
            json += "\"totalDevices\": " + std::to_string(sqlite3_column_int(stmt, 2)) + ",";
            json += "\"compromises\": " + std::to_string(sqlite3_column_int(stmt, 3));
        }
        sqlite3_finalize(stmt);
    }

    json += "}";
    return json;
}

// Generic stat updater
void increment_stat(const std::string& field) {
    std::string sql = "UPDATE system_stats SET " + field + " = " + field + " + 1 WHERE id=1;";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

// API used by Analyzer
void increment_suspicious() {
    increment_stat("suspicious_detected");
    broadcast(get_stats());
}

void increment_killed() {
    increment_stat("processes_killed");
    increment_stat("compromises");
    broadcast(get_stats());
}

// Run the WebSocket++ server
void start_websocket_server() {
    init_db();

    ws_server.set_open_handler([](websocketpp::connection_hdl hdl) {
        connections.insert(hdl);
        ws_server.send(hdl, get_stats(), websocketpp::frame::opcode::text);
    });

    ws_server.set_close_handler([](websocketpp::connection_hdl hdl) {
        connections.erase(hdl);
    });

    ws_server.set_message_handler([](websocketpp::connection_hdl hdl, server::message_ptr msg) {
        std::cout << "Message received: " << msg->get_payload() << std::endl;
    });

    ws_server.init_asio();
    ws_server.listen(9002);
    ws_server.start_accept();

    std::cout << "WebSocket++ server running on port 9002\n";
    ws_server.run();

    sqlite3_close(db);
}
