#include "fileextensionchecker.h"

using namespace std;

// Static variable definitions
const unordered_set<string> FileExtensionChecker::known_zips = {".zip", ".rar", ".7z"};
const unordered_set<string> FileExtensionChecker::known_suspicious = {".exe", ".bat", ".js"};
const unordered_set<string> FileExtensionChecker::known_images = {".jpg", ".jpeg", ".png", ".gif", ".bmp"};


bool FileExtensionChecker::is_zip(const string& ext) const {
    return false;
}

bool FileExtensionChecker::is_suspicious(const string& ext) const {
    return false;
}

bool FileExtensionChecker::is_image(const string& ext) const {
    return false;
}