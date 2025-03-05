import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import padding
import secrets

# Function to generate a random encryption key
def generate_key():
    return secrets.token_bytes(32)  # AES-256

# Function to encrypt a file
def encrypt_file(file_path, key):
    try:
        print(f"Encrypting: {file_path}")  # Print the file name before encryption

        # Open the file in binary mode
        with open(file_path, 'rb') as file:
            file_data = file.read()

        if not file_data:
            print(f"File is empty or could not be read: {file_path}")
            return

        # Pad the data to be multiple of block size (128 bits for AES)
        padder = padding.PKCS7(128).padder()
        padded_data = padder.update(file_data) + padder.finalize()

        # Create a random IV for each encryption
        iv = secrets.token_bytes(16)

        # Set up the AES cipher in CBC mode
        cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
        encryptor = cipher.encryptor()

        # Encrypt the data
        encrypted_data = encryptor.update(padded_data) + encryptor.finalize()

        # Write the encrypted data back to the file (overwrite the original or save as new)
        with open(file_path + '.enc', 'wb') as encrypted_file:
            encrypted_file.write(iv + encrypted_data)  # Prepend the IV to the encrypted file

        print(f"Encrypted: {file_path}")
    except Exception as e:
        print(f"Failed to encrypt {file_path}: {e}")

# Function to encrypt all files in a folder and subfolders
def encrypt_folder(folder_path, key):
    if not os.path.exists(folder_path):
        print(f"Folder does not exist: {folder_path}")
        return

    print(f"Starting encryption in folder: {folder_path}")
    # Walk through the directory
    for dirpath, dirnames, filenames in os.walk(folder_path):
        print(f"Processing directory: {dirpath}")
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            print(f"Found file: {file_path}")
            encrypt_file(file_path, key)

# Main function
if __name__ == "__main__":
    folder_path = "./encryption_zone/files2encrypt"  # Always use the "files2encrypt" folder
    key = generate_key()
    encrypt_folder(folder_path, key)
    print("All files encrypted.")
    print("Encryption key:", key.hex())  # Convert bytes to hex string for printing
