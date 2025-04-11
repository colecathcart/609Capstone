# ENSF 609 Capstone: Detecting Ransomware on Linux

A software application that detects and removes ransomware on Linux (Ubuntu) systems. Developed for Cisco Systems as a proof of concept for ransomware detection, 
this program utilizes Shannon entropy analysis and the NIST Monobit Frequency Test to identify potentially malicious encryptions, shut down offending process groups
and remove exeutables. The program also creates hashes of suspicious executables for use with a theoretical connected antivirus software. The solution is implemented in object-oriented C++ with a focus on optimizing memory and computation performance. Additional features include a GUI
for the user to easily configure the program and an administrative dashboard to track the safety of multiple computers on a network. This repository also includes an adapted version of the [GonnaCry](https://github.com/tarcisio-marinho/GonnaCry) ransomware that was used to evaluate the effectiveness of the solution.

An application demo of the modified GonnaCry ransomware is available at this link:

[Ransomware Demo](https://drive.google.com/file/d/1hIWWA_ZhmGaZqhzdlf3ynPu0LOJaWLPk/view?usp=sharing)

An demo of the same ransomware being stopped by the detector is available at this link:

[Detector Demo](https://drive.google.com/file/d/1rDO_HqtsSHD4BkOUQF_5ZmCVJdJY73wC/view?usp=sharing)

## Project File Structure

- `app/`: Contains the detector application code and the Makefile for compiling the project, as well as the unit tests used for CI and some 3rd-party libraries included as sub-repositories (C++)
- `dashboard/`: Contains the administrator dashboard code (ReactJS)
- `encryption_zone/`: Contains the modified GonnaCry code used for testing the ransomware detector (Python)
- `GUI/`: Contains the GUI application code (C++ Qt)

## Instructions for Compiling and Running
Each of the four directories is a standalone application with its own directions for compiling. This project uses many libraries which are not 
included in a standard Ubuntu installation, so we have compiled many of them into a list for ease of setup.

**Prerequesites**
- Ubuntu 24.04 LTS

If your system has the above prerequesites, simply clone the repository, navigate to the project directory and run `sudo apt install requirements.txt` from a terminal.
Then continue with the steps below for compiling individual programs.

### Ransomware Detector
To compile the main application, use the provided Makefile. Navigate to the `app` directory and run the following commands:

- `make`: Compiles the project executable.
- `make websocket_server_app`: Compiles an executable for communicating with the dashboard.

The ransomware detector can now be run as-is from command line, like so: 

`sudo ./ransomware_detector <directory>`

- Replace `<directory>` with the directory for the detector to watch. It is reccomended to watch the entire filesystem, ie `/`.

Log messages will appear in the terminal. Note that the detector must be run as sudo.

If you are runing the administrator dashboard (see below) and would like to connect the local detector, you must first run the
Websocket executable before starting the detector: `./websocket_server_app`. You should now see your device connected to the dashboard.

### GonnaCry
Because of the nature of Python libraries, there are additional instructions to install packages in a virtual environment. Please follow the detailed instructions
located in `encryption_zone/GonnaCry/README.md`.

### GUI
Due to difficulties encountered with linking the Qt packages, you must create an account and download the Qt installer in order to run the GUI:
1. Create a free Qt account [here](https://login.qt.io/register). You will need to login with this account after downloading the installer.
2. Download the open source Qt installer [here](https://www.qt.io/download-qt-installer-oss).
3. Give the installer execution permissions and then run the installer. You will be prompted to login with your account.

Once the install is complete, you must take note of the path to the `qmake` executable on your system. It should be located at
`/home/<user>/Qt/6.9.0/<architecture>/bin/qmake`
- Replace `<user>` with your computer username. Replace `<architecture>` with either `gcc_64` or `gcc_arm64` depending on the version of Qt you installed

Navigate into the /GUI directory inside the project folder and enter the full path you just determined into the terminal. This will generate a makefile (this only needs to be done once). To activate the GUI, run `sudo ./GUI` from the same directory. 
- To start the detector, click the 'Start detector' button in the interface. The directory allowlist and extension blacklist can be edited from their corresponding tabs (the detector must be restarted to take effect).
- Note that the GUI and ransomware detector are standalone programs by design. This means that the ransomware detector must be compiled into an executable before running the GUI (see above steps).
- Additionally, closing the GUI window will not close the detector (this is intentional). To stop the detector, please click the corresponding button in the interface.
- Note that although the GUI contains an option to use honey files, the detector currently only uses entropy analysis.
- Currently, the GUI will **not** automatically start the websocket client when it starts the detector. If you want to connect to the dashboard, please activate that executable first (see **Ransomware Detector** section above).

### Dashboard
The dashboard is intended to be used by a system administrator to monitor the health of a local network of computers. It will only recognize devices on the local network.

Navigate into the `/dashboard` directory and run the command `npm install` to automatically install all required packages. Then run `npm run dev` to open the dashboard in a web browser. To connect a device to the detector, follow the steps outlined in the **Ransomware Detector** section above.
