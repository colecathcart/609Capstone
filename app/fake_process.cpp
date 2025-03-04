#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    // Create a new process group
    setpgid(0, 0);  // Sets the process group ID of this process to its PID

    printf("Fake Ransomware Started | PID: %d | PGID: %d\n", getpid(), getpgid(0));

    // Fork multiple child processes
    for (int i = 0; i < 3; i++) {  // Adjust number of children
        pid_t pid = fork();

        if (pid == 0) {  
            // Child process
            printf("Child process started | PID: %d | PGID: %d\n", getpid(), getpgid(0));
            while (1) sleep(1000);  // Keep child running
        } 
        else if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }
    }

    // Keep parent process alive
    while (1) sleep(1000);

    return 0;
}
