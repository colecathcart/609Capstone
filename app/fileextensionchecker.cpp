#include "fileextensionchecker.h"

using namespace std;

// Static variable definitions
const unordered_set<string> FileExtensionChecker::known_compressed = {".zip", ".rar", ".7z"};
const unordered_set<string> FileExtensionChecker::known_suspicious = {".crypt", ".crypted", ".cryp1", ".crypz", ".enc",".enc0", ".enc1", ".locked", ".lock", ".block", ".secure"};
const unordered_set<string> FileExtensionChecker::known_images = {".jpg", ".jpeg", ".png", ".gif", ".tiff", ".tif", ".bmp"};

FileExtensionChecker::FileExtensionChecker() {}

string FileExtensionChecker:: get_extension(const string& filepath) const {
    size_t pos = filepath.rfind('.');
    if (pos != string::npos) {
        string file_extension = filepath.substr(pos);
        return file_extension;
    }
    return "";
}

bool FileExtensionChecker::is_compressed(const string& filepath) const {
    string file_extension = get_extension(filepath);
    if (file_extension != "") {
        bool is_file_compressed = known_compressed.count(file_extension) == 1;
        return is_file_compressed;
    }
    return false;
}

bool FileExtensionChecker::is_suspicious(const string& filepath) const {
    string file_extension = get_extension(filepath);
    if (file_extension != "") {
        bool is_file_suspicious = known_suspicious.count(file_extension) == 1;
        return is_file_suspicious;
    }
    return false;
}

bool FileExtensionChecker::is_image(const string& filepath) const {
    string file_extension = get_extension(filepath);
    if (file_extension != "") {
        bool is_file_an_image = known_images.count(file_extension) == 1;
        return is_file_an_image;
    }
    return false;
}