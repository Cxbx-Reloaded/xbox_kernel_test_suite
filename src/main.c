#include <hal/debug.h>
#include <pbkit/pbkit.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "util/output.h"
#include "util/misc.h"
#include "util/vector.h"
#include "util/string_extra.h"
#include "global.h"
#include "include/func_table.h"

int load_conf_file(char *file_path)
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
    if(handle == INVALID_HANDLE_VALUE) {
        print("Could not open config file '%s' for read", file_path);
        return -1;
    }

    DWORD file_size = GetFileSize(handle, NULL);
    if(file_size == INVALID_FILE_SIZE) {
        print("ERROR: Could not get file size for %s", file_path);
        return -1;
    }

    char* buffer = (char *)malloc(file_size);
    if(buffer == NULL) {
        print("Malloc failed for file_size %u", file_size);
        return -1;
    }

    DWORD bytes_read = 0;
    BOOL result = ReadFile(handle, buffer, file_size, &bytes_read, NULL);
    if(result == 0 || bytes_read != file_size) {
        print("Read failed for config file. result = %d, read = %u", file_size, bytes_read);
        return -1;
    }

    CloseHandle(handle);

    char *line;
    char *rest = buffer;
    while ((line = strtok_r(rest, "\n", &rest))){
        char *current_key = strtok(line, "=");
        if(strcmp("seed", current_key) == 0){
            seed = strtoul(strtok(NULL, "\n"), NULL, 16);
        }
        if(strcmp("tests", current_key) == 0){
            char *current_test;
            char *tests = strtok(NULL, "\n");
            while((current_test = strtok_r(tests, ",", &tests))) {
                vector_append(&tests_to_run, strtol(current_test, NULL, 16));
            }
        }
    }

    free(buffer);
    return 0;
}

static void run_tests()
{
    print("Random seed used is %u", seed);
    if(tests_to_run.size == 0) {
        print("No Specific tests specified. Running all tests (Single Pass).");
        print("-------------------------------------------------------------");
        int table_size = ARRAY_SIZE(kernel_thunk_table);
        for(int k=0;k<table_size;k++){
            kernel_thunk_table[k]();
        }
    }
    else{
        print("Config File Was Loaded. Only running requested tests.");
        print("-----------------------------------------------------");
        for(int k=0; k<tests_to_run.size; k++){
            kernel_thunk_table[vector_get(&tests_to_run, k)]();
        }
    }
    print("------------------------ End of Tests -----------------------");
}

void main(void){

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    switch(pb_init()){
        case 0: break;
        default:
            Sleep(2000);
            XReboot();
            return;
    }

    pb_show_debug_screen();

    vector_init(&tests_to_run);
    load_conf_file("D:\\config.txt");
    open_output_file("D:\\kernel_tests.log");

    print("Kernel Test Suite");
    run_tests();


    vector_free(&tests_to_run);
    close_output_file();

    Sleep(10000);
    pb_kill();
}
