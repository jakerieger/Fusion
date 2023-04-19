#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define PLATFORM "windows"
#define UTF8_SUPPORT 0
#elif __linux__
#define PLATFORM "linux_unknown"
#define UTF8_SUPPORT 1
#elif __APPLE__
#define PLATFORM "darwin"
#define UTF8_SUPPORT 1
#else
#define PLATFORM "unknown"
#define UTF8_SUPPORT 0
#endif

char* get_fuse_bin_path();
char* get_fuse_watch_bin_path();
char* get_fuse_vm_config_path();

int main(void) {
    printf("%s\n", PLATFORM);
    return 0;
}