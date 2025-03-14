#include "file_extension_checker.h"
#include <fstream>
#include <iostream>

using namespace std;

// Static variable definitions
const unordered_set<string> FileExtensionChecker::known_high_ent = load_known_extensions("data/high_entropy_files.txt");
const unordered_set<string> FileExtensionChecker::known_suspicious = load_known_extensions("data/known_encrypted.txt");

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
    cout << mime_type << endl;
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
    bool is_high_ent = known_high_ent.find(file_extension) != known_high_ent.end();
    return is_high_ent;
    return false;
}

bool FileExtensionChecker::is_suspicious(const string& filepath) const {
    string file_extension = get_extension(filepath);
    if (file_extension != "") {
        bool is_file_suspicious = known_suspicious.find(file_extension) != known_suspicious.end();
        return is_file_suspicious;
    }
    return false;
}