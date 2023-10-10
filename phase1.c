#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMD_SIZE 1024
#define ARG_SIZE 100
#define MAX_PIPES 3

void execute_cmd(char **args, int *in_fd, int *out_fd) {
    if (in_fd) {
        dup2(*in_fd, 0);
        close(*in_fd);
    }

    if (out_fd) {
        dup2(*out_fd, 1);
        close(*out_fd);
    }

    if (strcmp(args[0], "ls") == 0) {
        execvp("ls", args);
    } else if (strcmp(args[0], "pwd") == 0) {
        execvp("pwd", args);
    } else if (strcmp(args[0], "mkdir") == 0) {
        execvp("mkdir", args);
    } else if (strcmp(args[0], "echo") == 0) {
        execvp("echo", args);
    } else if (strcmp(args[0], "cat") == 0) {
        execvp("cat", args);
    } else if (strcmp(args[0], "rm") == 0) {
        execvp("rm", args);
    } else if (strcmp(args[0], "cd") == 0) {
        chdir(args[1]);
    } else if (strcmp(args[0], "file") == 0) {
        execvp("file", args);
    } else if (strcmp(args[0], "cp") == 0) {
        execvp("cp", args);
    } else if (strcmp(args[0], "mv") == 0) {
        execvp("mv", args);
    } else if (strcmp(args[0], "ps") == 0) {
        execvp("ps", args);
    } else if (strcmp(args[0], "grep") == 0) {
        execvp("grep", args);
    } else if (strcmp(args[0], "wc") == 0) {
        execvp("wc", args);
    } else if (strcmp(args[0], "sort") == 0) {
        execvp("sort", args);
    } else if (strcmp(args[0], "find") == 0) {
        execvp("find", args);
    } else {
        printf("Command not found\n");
        exit(EXIT_FAILURE);
    }
}

int main() {
    char cmd[CMD_SIZE];
    char *args[ARG_SIZE];

    while (1) {
        printf("myshell> ");
        fgets(cmd, CMD_SIZE, stdin);
        cmd[strcspn(cmd, "\n")] = 0; // Remove trailing newline
            
        if (strcmp(cmd, "exit") == 0){
            exit(EXIT_SUCCESS);
        }
        
        // Check for pipes and split commands
        char *commands[MAX_PIPES + 1];
        int i = 0;
        commands[i] = strtok(cmd, "|");
        while (commands[i] != NULL) {
            i++;
            commands[i] = strtok(NULL, "|");
        }

        int num_commands = i;
        int pipe_fds[2 * MAX_PIPES];
        for (i = 0; i < num_commands - 1; i++) {
            if (pipe(pipe_fds + (2 * i)) == -1) {
                perror("Pipe failed");
                return 1;
            }
        }

        for (i = 0; i < num_commands; i++) {
            char *part = commands[i];
            int j = 0;
            args[j] = strtok(part, " ");
            while (args[j] != NULL) {
                j++;
                args[j] = strtok(NULL, " ");
            }

            pid_t pid = fork();
            if (pid == 0) {
                execute_cmd(args, (i > 0) ? &pipe_fds[(i - 1) * 2] : NULL, (i < num_commands - 1) ? &pipe_fds[i * 2 + 1] : NULL);
            } else if (pid < 0) {
                perror("myshell");
            }

            wait(NULL);

            // Close write end as parent shouldn't write to the pipes
            if (i < num_commands - 1) {
                close(pipe_fds[i * 2 + 1]);
            }
        }

        // Close all pipes and wait for all child processes
        for (i = 0; i < 2 * (num_commands - 1); i++) {
            close(pipe_fds[i]);
        }
    }

    return 0;
}



