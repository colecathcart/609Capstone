#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

using namespace std;
namespace fs = std::filesystem;

// Function to generate a random encryption key
vector<unsigned char> generate_key() {
    vector<unsigned char> key(32); // AES-256
    if (!RAND_bytes(key.data(), key.size())) {
        throw runtime_error("Failed to generate key");
    }
    return key;
}

// Function to encrypt a file
void encrypt_file(const string& file_path, const vector<unsigned char>& key) {
    try {
        cout << "Encrypting: " << file_path << endl; // Print the file name before encryption

        // Open the file in binary mode
        ifstream file(file_path, ios::binary);
        if (!file) {
            throw runtime_error("File could not be opened");
        }

        vector<unsigned char> file_data((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();

        if (file_data.empty()) {
            cout << "File is empty or could not be read: " << file_path << endl;
            return;
        }

        // Pad the data to be multiple of block size (128 bits for AES)
        int block_size = AES_BLOCK_SIZE;
        int padding_length = block_size - (file_data.size() % block_size);
        file_data.insert(file_data.end(), padding_length, static_cast<unsigned char>(padding_length));

        // Create a random IV for each encryption
        vector<unsigned char> iv(AES_BLOCK_SIZE);
        if (!RAND_bytes(iv.data(), iv.size())) {
            throw runtime_error("Failed to generate IV");
        }

        // Set up the AES cipher in CBC mode
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());

        vector<unsigned char> encrypted_data(file_data.size() + AES_BLOCK_SIZE);
        int out_len1, out_len2;
        EVP_EncryptUpdate(ctx, encrypted_data.data(), &out_len1, file_data.data(), file_data.size());
        EVP_EncryptFinal_ex(ctx, encrypted_data.data() + out_len1, &out_len2);
        EVP_CIPHER_CTX_free(ctx);

        // Write the encrypted data back to the file (overwrite the original or save as new)
        ofstream encrypted_file(file_path + ".enc", ios::binary);
        encrypted_file.write(reinterpret_cast<char*>(iv.data()), iv.size()); // Prepend the IV to the encrypted file
        encrypted_file.write(reinterpret_cast<char*>(encrypted_data.data()), out_len1 + out_len2);
        encrypted_file.close();

        cout << "Encrypted: " << file_path << endl;
    } catch (const exception& e) {
        cout << "Failed to encrypt " << file_path << ": " << e.what() << endl;
    }
}

// Function to encrypt all files in a folder and subfolders
void encrypt_folder(const string& folder_path, const vector<unsigned char>& key) {
    if (!fs::exists(folder_path)) {
        cout << "Folder does not exist: " << folder_path << endl;
        return;
    }

    cout << "Starting encryption in folder: " << folder_path << endl;
    // Walk through the directory
    for (const auto& dir_entry : fs::recursive_directory_iterator(folder_path)) {
        if (fs::is_regular_file(dir_entry)) {
            string file_path = dir_entry.path().string();
            cout << "Found file: " << file_path << endl;
            encrypt_file(file_path, key);
        }
    }
}

// Main function
int main() {
    string folder_path = "./files2encrypt"; // Always use the "files2encrypt" folder
    vector<unsigned char> key = generate_key();
    encrypt_folder(folder_path, key);
    cout << "All files encrypted." << endl;

    // Convert bytes to hex string for printing
    cout << "Encryption key: ";
    for (const auto& byte : key) {
        cout << hex << setw(2) << setfill('0') << (int)byte;
    }
    cout << endl;

    return 0;
}
