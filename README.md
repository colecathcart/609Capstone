# ENSF 609 Capstone: Detecting Ransomware on Linux

A software application that detects and removes ransomware on Linux (Ubuntu) systems.

An application demo of the modified GonnaCry ransomware is available at this link:

[Ransomware Demo](https://drive.google.com/file/d/1hIWWA_ZhmGaZqhzdlf3ynPu0LOJaWLPk/view?usp=sharing)

An demo of the same ransomware being stopped by the detector is available at this link:

[Detector Demo](https://drive.google.com/file/d/1rDO_HqtsSHD4BkOUQF_5ZmCVJdJY73wC/view?usp=sharing)

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

## Encryption Script

The `encrypt.py` script in the `encryption_zone` directory is used to encrypt files for testing the ransomware detector.

**WARNING**: If testing the ransomware detector using the `encrypt.py` script, do not delete the executable when prompted - this will remove Python from your environment.

Testing with `encrypt.cpp` is also possible, but currently requires manual compilation. 

## Features In-Progress

- GUI
- Performance Optimization
- Admin Monitoring Dashboard
- Honey Files
