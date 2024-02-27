#include <pbkit/pbkit.h>
#include <hal/video.h> // for XVideoSetMode
#include <hal/xbox.h> // for XReboot
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <bitset>

extern "C" {
#include "util/hardware.h"
#include "util/output.h"
#include "util/misc.h"
#include "util/string_extra.h"
#include "global.h"
#include "include/api_tests.h"
}

#ifndef GIT_VERSION
#define GIT_VERSION "unknown"
#endif

// To read line by line properly from all platforms' line-endings with their text editor software.
// Newline delimiter reader requires "\r\n" to find whichever character is found first.
// Windows (\r\n), POSIX (\n), (legacy) Mac OS (\r)
#define NEWLINE_DELIMITER "\r\n"

// defined in util/output.h file, used privately here only
extern "C" BOOL output_video;
// Initialize the actual default values here if the config file is either successfully loaded before reading inputs or it failed to load.
static void init_default_values()
{
    // NOTE: Pre-init is set to FALSE because video initialization doesn't occur until after the config file is loaded.
    output_video = TRUE;
}

static char *submitter = NULL;
#define LOG_NAME_PATH_DEFAULT "D:\\kernel_tests"
#define LOG_FILE_EXT ".log"
static const char* name_log_format = LOG_NAME_PATH_DEFAULT "-%s" LOG_FILE_EXT;
static char* name_log = NULL;

void load_name_file(const char* file_path)
{
    HANDLE handle = CreateFile(
        file_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (handle == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD file_size = GetFileSize(handle, NULL);
    if (file_size == INVALID_FILE_SIZE) {
        return;
    }

    char* buffer = (char *)malloc(file_size);
    if (buffer == NULL) {
        return;
    }

    DWORD bytes_read = 0;
    BOOL result = ReadFile(handle, buffer, file_size, &bytes_read, NULL);
    if (result == 0 || bytes_read != file_size) {
        free(buffer);
        return;
    }

    CloseHandle(handle);

    char *line;
    char *rest = buffer;
    if ((line = strtok_r(rest, NEWLINE_DELIMITER, &rest))) {
        size_t length = strlen(line);
        if (length) {
            name_log = (char*)calloc(length + 1, sizeof(char));
            strncpy(name_log, line, length);
        }
    }

    free(buffer);
}

static std::bitset<kernel_api_tests_size> tests_to_run;
static std::bitset<kernel_api_tests_size> tests_exclude;

int load_conf_file(const char *file_path)
{
    print("Trying to open config file: %s", file_path);
    HANDLE handle = CreateFile(
        file_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (handle == INVALID_HANDLE_VALUE) {
        print("Could not open config file '%s' for read", file_path);
        return -1;
    }

    DWORD file_size = GetFileSize(handle, NULL);
    if (file_size == INVALID_FILE_SIZE) {
        print("ERROR: Could not get file size for %s", file_path);
        return -1;
    }

    char* buffer = (char *)malloc(file_size);
    if (buffer == NULL) {
        print("Malloc failed for file_size %u", file_size);
        return -1;
    }

    DWORD bytes_read = 0;
    BOOL result = ReadFile(handle, buffer, file_size, &bytes_read, NULL);
    if (result == 0 || bytes_read != file_size) {
        print("Read failed for config file. result = %d, read = %u", file_size, bytes_read);
        free(buffer);
        return -1;
    }

    CloseHandle(handle);
    // Once the config file is loaded successfully, then initialize the actual default values (if any). 
    init_default_values();

    char *line;
    char *rest = buffer;
    while ((line = strtok_r(rest, NEWLINE_DELIMITER, &rest))) {
        char *current_key = strtok(line, "=");
        if (strcmp("seed", current_key) == 0) {
            seed = strtoul(strtok(NULL, NEWLINE_DELIMITER), NULL, 16);
        }
        if (strcmp("tests", current_key) == 0) {
            char *current_test;
            char *tests = strtok(NULL, NEWLINE_DELIMITER);
            while ((current_test = strtok_r(tests, ",", &tests))) {
                unsigned long value = strtoul(current_test, NULL, 16);
                if (value < kernel_api_tests_size) {
                    tests_to_run.set(value);
                }
            }
        }
        if (strcmp("tests-exclude", current_key) == 0) {
            char *current_test;
            char *tests = strtok(NULL, NEWLINE_DELIMITER);
            while ((current_test = strtok_r(tests, ",", &tests))) {
                unsigned long value = strtoul(current_test, NULL, 16);
                if (value < kernel_api_tests_size) {
                    tests_exclude.set(value);
                }
            }
        }
        if (strcmp("disable-video", current_key) == 0) {
            output_video = !strtoul(strtok(NULL, NEWLINE_DELIMITER), NULL, 16);
        }
        if (strcmp("submitter", current_key) == 0) {
            char *value = strtok(NULL, NEWLINE_DELIMITER);
            size_t length = strlen(value);
            submitter = (char*)calloc(length + 1, sizeof(char));
            strncpy(submitter, value, length);
        }
    }

    free(buffer);
    return 0;
}

static void run_tests()
{
    print("Random seed used is %u", seed);
    if (tests_to_run.none()) {
        print("No specific tests were requested. Running all tests.");
        // Enable all tests.
        tests_to_run.flip();
        if (tests_exclude.any()) {
            // Exclude any tests requested.
            tests_to_run &= ~tests_exclude;
            print("%zu test(s) will be excluded.", tests_exclude.count());
        }
    }
    else {
        print("A config file was loaded. Only running requested tests.");
        if (tests_exclude.any()) {
            const auto found_tests_exclude = tests_to_run & tests_exclude;
            // Exclude any tests requested.
            tests_to_run &= ~found_tests_exclude;
            print("%zu test(s) will be excluded.", found_tests_exclude.count());
        }
    }
    print("-------------------------------------------------------------");
    for (int i = 0; i < kernel_api_tests_size; i++) {
        if (tests_to_run.test(i)) {
            kernel_api_tests[i].func(i, kernel_api_tests[i].name);
        }
    }
    print("------------------------ End of Tests -----------------------");
}

void main(void)
{
    load_name_file("D:\\name.txt");
    char* output_file_name = (char*)"D:\\kernel_tests.log";
    // If name_log buffer is allocated, then we know it does have actual input.
    if (name_log) {
        size_t name_log_length = strlen(name_log);
        name_log_length += strlen(name_log_format) - 2; // exclude %s
        output_file_name = (char*)calloc(name_log_length + 1, sizeof(char));
        snprintf(output_file_name, name_log_length, name_log_format, name_log);
    }
    if (!open_output_file(output_file_name)) {
        return;
    }
    if (load_conf_file("D:\\config.txt")) {
        init_default_values();
    }

    if (output_video) {
        XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
        // If the pb_init call returns non-zero, then something went wrong from nxdk's end.
        if (pb_init()) {
            Sleep(2000);
            XReboot();
            return;
        }
        pb_show_debug_screen();
    }

    print("Kernel Test Suite");
    print("build: " GIT_VERSION);
    print("submitter: %s", (submitter ? submitter : ""));
    if (submitter) {
        free(submitter);
        submitter = NULL;
    }
    print("name: %s", (name_log ? name_log : ""));
    if (name_log) {
        free(name_log);
        name_log = NULL;
        free(output_file_name);
        output_file_name = NULL;
    }
    print("kernel: %hu.%hu.%hu.%hu",
          XboxKrnlVersion.Major,
          XboxKrnlVersion.Minor,
          XboxKrnlVersion.Build,
          XboxKrnlVersion.Qfe);
    print("hardware info: flags = 0x%08X, GPU rev = %hhu, MCPX rev = %hhu",
          XboxHardwareInfo.Flags,
          XboxHardwareInfo.GpuRevision,
          XboxHardwareInfo.McpRevision);
    char pic_version[4];
    getPICVersion(pic_version);
    print("PIC version: %s (%s)", pic_version, getConsoleType(pic_version));
    run_tests();

    close_output_file();

    if (output_video) {
        Sleep(10000);
        pb_kill();
    }
}
