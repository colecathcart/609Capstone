import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import padding

# Function to decrypt a file
def decrypt_file(file_path, key):
    # Open the encrypted file
    with open(file_path, 'rb') as encrypted_file:
        encrypted_data = encrypted_file.read()

    # Extract IV (first 16 bytes) and encrypted content
    iv = encrypted_data[:16]
    actual_encrypted_data = encrypted_data[16:]

    # Set up AES cipher in CBC mode with the IV
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    decryptor = cipher.decryptor()

    # Decrypt the file data
    decrypted_padded_data = decryptor.update(actual_encrypted_data) + decryptor.finalize()

    # Remove padding
    unpadder = padding.PKCS7(128).unpadder()
    decrypted_data = unpadder.update(decrypted_padded_data) + unpadder.finalize()

    # Save the decrypted data to a new file (removing .enc extension)
    original_file_path = file_path[:-4]  # Remove ".enc" extension
    with open(original_file_path, 'wb') as decrypted_file:
        decrypted_file.write(decrypted_data)

    # Optionally delete the encrypted file after successful decryption
    os.remove(file_path)

    print(f"Decrypted: {original_file_path}")

# Function to decrypt all files in a folder and subfolders
def decrypt_folder(folder_path, key):
    for dirpath, _, filenames in os.walk(folder_path):
        for filename in filenames:
            if filename.endswith('.enc'):  # Only process encrypted files
                file_path = os.path.join(dirpath, filename)
                decrypt_file(file_path, key)

# Main function
if __name__ == "__main__":
    folder_path = input("Enter the folder path to decrypt: ")
    
    # You need to enter the same key used for encryption
    key_hex = input("Enter the encryption key (hex format): ")
    key = bytes.fromhex(key_hex)  # Convert hex key back to bytes
    
    decrypt_folder(folder_path, key)
    print("All files decrypted.")
