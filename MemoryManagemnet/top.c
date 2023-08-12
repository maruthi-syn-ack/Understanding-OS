#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 128

// Custom memory allocation functions for tracking
void* custom_malloc(size_t size) {
    void* ptr = malloc(size);
    // Record memory allocation event here
    return ptr;
}

void custom_free(void* ptr) {
    // Record memory deallocation event here
    free(ptr);
}

// Function to retrieve memory consumption from /proc
unsigned long get_memory_usage(int pid) {
    char proc_path[MAX_PATH_LENGTH];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/status", pid);

    char line[256];
    FILE* file = fopen(proc_path, "r");
    unsigned long memory_usage = 0;

    if (file) {
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "VmRSS:", 6) == 0) {
                sscanf(line, "VmRSS: %lu", &memory_usage);
                break;
            }
        }
        fclose(file);
    }

    return memory_usage;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
        return 1;
    }

    int pid = atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Invalid PID\n");
        return 1;
    }

    // Replace standard malloc and free with custom functions
    #define malloc(size) custom_malloc(size)
    #define free(ptr) custom_free(ptr)

    while (1) {
        unsigned long memory_usage = get_memory_usage(pid);
        printf("Memory usage for process %d: %lu KB\n", pid, memory_usage);

        // Sleep for a short interval before updating again
        usleep(500000); // 500 milliseconds
    }

    return 0;
}
