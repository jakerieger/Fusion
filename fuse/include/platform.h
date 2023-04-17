// Copyright © 2023 Gymate, Inc.
//
// Licensed under the GNU General Public License v3.0

/**
 * @brief This file handles defining flags and constants for platform-specific purposes
 */
#ifndef FUSION_PLATFORM_H
#define FUSION_PLATFORM_H

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

static const char* get_platform_config_dir() {
    if (strcmp(PLATFORM, "windows") == 0) {
        char* homedir = getenv("USERPROFILE");
        char fullpath[256];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", homedir, ".fusion");
        return strdup(fullpath);
    } else if (strcmp(PLATFORM, "linux_unknown") == 0 || strcmp(PLATFORM, "darwin") == 0) {
        char* homedir = getenv("HOME");
        char fullpath[256];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", homedir, ".fusion");
        return strdup(fullpath);
    } else {
        return "./";
    }
}

#endif