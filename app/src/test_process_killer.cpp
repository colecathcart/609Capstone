#include "../include/process_killer.h"
#include <iostream>
#include <vector>

int main() {
    int pid;
    std::cout << "Enter the PID of the process to kill: ";
    std::cin >> pid;

    ProcessKiller killer(pid);

    killer.killFamily();

    return 0;
}
