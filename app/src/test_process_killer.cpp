#include "../include/process_killer.h"
#include <iostream>
#include <vector>

int main() {
    int pid;
    std::cout << "Enter the PID of the process to kill: ";
    std::cin >> pid;

    ProcessKiller killer;

    string executable_path = killer.getExecutablePath(pid); // can only retrieve path before killing process group
    killer.killFamily(pid);
    killer.removeExecutable(executable_path);

    return 0;
}
