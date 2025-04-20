#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "http/server.h"
#include "db/database.h"

static volatile int keep_running = 1;

static void handle_signal(int signum) {
    (void)signum;
    keep_running = 0;
}

int main(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (db_init("todo.db") != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return EXIT_FAILURE;
    }

    if (http_server_init(8080) != 0) {
        fprintf(stderr, "Failed to initialize HTTP server\n");
        db_cleanup();
        return EXIT_FAILURE;
    }

    printf("Todo REST API server running on port 8080...\n");

    while (keep_running) {
        http_server_process();
    }

    http_server_cleanup();
    db_cleanup();
    printf("\nServer shutdown complete\n");

    return EXIT_SUCCESS;
} 