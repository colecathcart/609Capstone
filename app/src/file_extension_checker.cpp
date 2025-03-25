#include "file_extension_checker.h"
#include <fstream>
#include <iostream>

using namespace std;

#define WHITELIST_HIGH_ENT "data/whitelist_high_ent.txt"
#define BLACKLIST_EXT "data/blacklist_extensions.txt"

// Static variable definitions
const unordered_set<string> FileExtensionChecker::good_high_ent = load_known_extensions(WHITELIST_HIGH_ENT);
const unordered_set<string> FileExtensionChecker::bad_extensions = load_known_extensions(BLACKLIST_EXT);

unordered_set<string> FileExtensionChecker::load_known_extensions(const string& filename) {
    unordered_set<string> extensions;
    ifstream file(filename);
    string extension;

    if (file.is_open()) {
        while (getline(file, extension)) {
            if (!extension.empty()) {
                extensions.insert(extension);
            }
        }
        file.close();
    } else {
        cerr << "Error opening file: " << filename << endl;
    }   
    return extensions;
}

FileExtensionChecker::FileExtensionChecker() {
    magic = magic_open(MAGIC_MIME_TYPE);
    if (magic == nullptr) {
        cerr << "Error initializing libmagic" << endl;
    }
    if (magic_load(magic, nullptr) == -1) {
        cerr << "Error: " << magic_error(magic) << endl;
        magic_close(magic);
    }
}

string FileExtensionChecker::get_type(const string& filepath) const {
    const char* mime_type = magic_file(magic, filepath.c_str());
    // cout << mime_type << endl;
    if(!mime_type) {
        return "?";
    }

    string mime_string(mime_type);
    if(mime_string.find("application/") == 0) {
        return mime_string.substr(12);
    }
    size_t pos = filepath.find('/');
    return mime_string.substr(0, pos);
}

string FileExtensionChecker::get_extension(const string& filepath) const {
    size_t pos = filepath.rfind('.');
    if (pos != string::npos) {
        string file_extension = filepath.substr(pos + 1);
        return file_extension;
    }
    return "";
}

bool FileExtensionChecker::needs_monobit(const string& filepath) const {
    string file_extension = get_type(filepath);
    bool is_high_ent = good_high_ent.find(file_extension) != good_high_ent.end();
    return is_high_ent;
}

bool FileExtensionChecker::is_blacklist_extension(const string& filepath) const {
    string file_extension = get_extension(filepath);
    if (file_extension != "") {
        bool is_file_suspicious = bad_extensions.find(file_extension) != bad_extensions.end();
        return is_file_suspicious;
    }
    return false;
}