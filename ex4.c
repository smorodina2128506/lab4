#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

extern char **environ;

static void run(char *argv[]) {
    if (strchr(argv[0], '/')) {
        execve(argv[0], argv, environ);
    } else {
        const char *dirs[] = {"/bin/", "/usr/bin/", "/usr/local/bin/", NULL};
        char path[512];
        for (int i = 0; dirs[i]; i++) {
            snprintf(path, sizeof(path), "%s%s", dirs[i], argv[0]);
            execve(path, argv, environ);
        }
    }
    perror(argv[0]);
    exit(1);
}

int main(void) {
    char line[1024];

    while (1) {
        while (waitpid(-1, NULL, WNOHANG) > 0);

        printf("myshell> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;

        char *argv[MAX_ARGS];
        int argc = 0;
        char *tok = strtok(line, " \t\n");
        while (tok && argc < MAX_ARGS - 1) {
            argv[argc++] = tok;
            tok = strtok(NULL, " \t\n");
        }
        argv[argc] = NULL;

        if (argc == 0) continue;
        if (strcmp(argv[0], "exit") == 0) break;

        int background = 0;
        if (strcmp(argv[argc - 1], "&") == 0) {
            background = 1;
            argv[--argc] = NULL;
            if (argc == 0) continue;
        }

        pid_t pid = fork();
        if (pid < 0) { perror("fork"); continue; }
        if (pid == 0) run(argv);

        if (background) printf("[background] PID=%d\n", pid);
        else waitpid(pid, NULL, 0);
    }
    return 0;
}