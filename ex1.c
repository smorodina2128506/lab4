#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

static void busy_work(void) {
    volatile unsigned long x = 0;
    for (unsigned long i = 0; i < 200000000UL; i++) {
        x += i;
    }
}

static void print_time(const char *name, clock_t start) {
    clock_t end = clock();
    double ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("[%s] execution time: %.2f ms\n", name, ms);
}

int main(void) {
    pid_t p1 = fork();
    if (p1 < 0) {
        perror("fork");
        return 1;
    }
    if (p1 == 0) {
        clock_t start = clock();
        printf("[child 1] PID=%d, PPID=%d\n", getpid(), getppid());
        busy_work();
        print_time("child 1", start);
        exit(0);
    }

    clock_t main_start = clock();

    pid_t p2 = fork();
    if (p2 < 0) {
        perror("fork");
        return 1;
    }
    if (p2 == 0) {
        clock_t start = clock();
        printf("[child 2] PID=%d, PPID=%d\n", getpid(), getppid());
        busy_work();
        print_time("child 2", start);
        exit(0);
    }

    printf("[main] PID=%d, PPID=%d\n", getpid(), getppid());
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    print_time("main", main_start);
    return 0;
}