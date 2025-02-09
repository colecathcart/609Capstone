#include "file_extension_checker.h"
#include <fstream>
#include <iostream>

using namespace std;

// Static variable definitions
const unordered_set<string> FileExtensionChecker::known_compressed = load_known_extensions("known_extensions/compressed.txt");
const unordered_set<string> FileExtensionChecker::known_suspicious = load_known_extensions("known_extensions/suspicious.txt");
const unordered_set<string> FileExtensionChecker::known_images = load_known_extensions("known_extensions/images.txt");

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

FileExtensionChecker::FileExtensionChecker() {}

string FileExtensionChecker::get_extension(const string& filepath) const {
    size_t pos = filepath.rfind('.');
    if (pos != string::npos) {
        string file_extension = filepath.substr(pos + 1);
        return file_extension;
    }
    return "";
}

bool FileExtensionChecker::is_compressed(const string& filepath) const {
    string file_extension = get_extension(filepath);
    if (file_extension != "") {
        bool is_file_compressed = known_compressed.find(file_extension) != known_compressed.end();
        return is_file_compressed;
    }
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

bool FileExtensionChecker::is_image(const string& filepath) const {
    string file_extension = get_extension(filepath);
    if (file_extension != "") {
        bool is_file_an_image = known_images.find(file_extension) != known_images.end();
        return is_file_an_image;
    }
    return false;
}