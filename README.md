# ENSF 609 Capstone: Detecting Ransomware on Linux

A software application that detects and removes ransomware on Linux (Ubuntu) systems.

An application demo as of March 10th, 2025 is available at this link:

[Demo Video](https://drive.google.com/file/d/1_kg18xHsj8cp9Rjf7lbe5lF6xD2rqKeS/view?usp=sharing)

## Project Structure

- `app/`: Contains the main application code and the Makefile for compiling the project.
- `encryption_zone/`: Contains the encryption script used for testing the ransomware detector.
  - `encrypt.py`: Python script to encrypt files in a specified folder and its subfolders.
  - `encrypt.cpp`: .cpp version of the Python script that can be compiled into an executable.
  - `files2encrypt/`: Contains the files that will be encrypted.

## Compilation Instructions

To compile the main application, use the provided Makefile. Navigate to the `app` directory and run the following commands:

- `make`: Compiles the project executable.
- `make clean`: Removes generated executables and object files.
- `make no_obj`: Compiles the executable and removes object files.

If you create new C++ files, they will need to be added to the Makefile.

## Encryption Script

The `encrypt.py` script in the `encryption_zone` directory is used to encrypt files for testing the ransomware detector.
Testing with `encrypt.cpp` is also possible, but currently requires manual compilation. 

