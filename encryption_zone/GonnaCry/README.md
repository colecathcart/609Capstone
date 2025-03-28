# Python GonnaCry version

This directory contains the Python *adapted* code of the GonnaCry ransomware

**Be aware running GonnaCry/main.py or GonnaCry/dist/gonnacry in your computer may cause harm. Always use a VM with a snapshot backup.**

# How this version works:

https://medium.com/@tarcisiomarinho/ransomware-encryption-techniques-696531d07bb9

# Work flow

Gonnacry encrypt all files and call Daemon

Daemon encrypt new files, calls decryptor and change wallpaper

Decryptor try to communicate to server to send the Client private key wich is encrypted. This function has been adapted to function locally instead of involving a server.

# Requirements 

Ensure you have navigated into encryption_zone/GonnaCry directory before executing any of the code below:

Before installing the pycrypto library and requests, a virtual python environment is required for Virtual Machine:
   
    
    python3 -m venv venvgonnacrysandbox
    
    source venvgonnacrysandbox/bin/activate
    

GonnaCry requires the pycrypto library and requests, installation below

    pip install -r requirements.txt

You may also need to install pyinstaller:

    pip install pyinstaller

# Compiling the code

    pyinstaller -F --clean main.py -n gonnacry

    pyinstaller -F --clean decryptor.py -n decryptor

# Running the code
Ensure you unzip files2encrypt.zip **inside** encryption_zone/GonnaCry directory and navigate there with your terminal.

To encrypt all files in files2encrypt: directly run main.py, or:
    
    dist/gonnacry

To decrypt your files: directly run decryptor.py, or:

    dist/decryptor

# Objectives:
- [x] encrypt all user files with AES-256-CBC.
- [x] Random AES key and IV for each file.
- [x] Works even without internet connection.
- [ ] Communication with the server to decrypt Client-private-key.
- [x] encrypt AES key with client-public-key RSA-2048.
- [x] encrypt client-private-key with RSA-2048 server-public-key.
- [x] Change computer wallpaper -> Gnome, Unity, LXDE, KDE, XFCE.
- [x] Decryptor that simulate communication to server to send keys and decrypt files.
- [ ] python webserver
- [x] Daemon
- [ ] Dropper

# GonnaCry src files

| File          | description   |
| ------------- |:-------------:|
| main.py      | GonnaCry start file|
| daemon.py     | dropped by main.py and run |
| dropper.py    | drop the malware on the computer |
| decryptor.py  | communicate with the server (simulated), decrypt keys and files|
| symmetric.py      |AES encryption|
| asymmetric.py | RSA encryption |
| generate_keys.py | Generate random AES keys|
| persistence.py | Persistence routines for linux OS|
| get_files.py | Find files to be encrypted|
| environment.py| environment variables|
| variables.py | Images and malware binaries|
| utils.py | Utilities routines|
