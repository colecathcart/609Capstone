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
    # Open the file in binary mode
    with open(file_path, 'rb') as file:
        file_data = file.read()

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

# Function to encrypt all files in a folder and subfolders
def encrypt_folder(folder_path, key):
    # Walk through the directory
    for dirpath, dirnames, filenames in os.walk(folder_path):
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            encrypt_file(file_path, key)

# Main function
if __name__ == "__main__":
    folder_path = input("Enter the folder path to encrypt: ")
    key = generate_key()  # You can save the key securely or print it for testing
    encrypt_folder(folder_path, key)
    print("All files encrypted.")
