#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/xbox.h>
#include <hal/fileio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output.h"
#include "global.h"
#include "func_table.h"
#include "vector.h"

int load_conf_file(char *config_file_path) {
    int handle;
    unsigned int file_size = 0;
    char *buffer;
    unsigned int read = 0;
    int result;

    print("Trying to open config file: %s", config_file_path);

    result = XCreateFile(&handle,
        config_file_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL);
    if(result != 0) {
        print("Could not open config file '%s' for read", config_file_path);
        return -1;
    }

    XGetFileSize(handle, &file_size);

    buffer = malloc(file_size);
    if(buffer == NULL) {
        print("Malloc failed for file_size %u", file_size);
        return -1;
    }

    result = XReadFile(handle, buffer, file_size, &read);
    if(result == 0 || read != file_size) {
        print("Read failed for config file. result = %d, read = %u", file_size, read);
        return -1;
    }

    XCloseHandle(handle);

    char *line;
    char *rest = buffer;
    while ((line = strtok_r(rest, "\n", &rest))){
        char *current_key = strtok(line, "=");
        if(strcmp("is_emu", current_key) == 0){
            is_emu = strtol(strtok(NULL, "\n"), NULL, 16);
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

static void run_tests() {
    if(is_emu) {
        print("Running tests in emulator mode");
    }
    else {
        print("Running tests in real hardware mode");
    }
    if(tests_to_run.size == 0) {
        print("No Specific tests specified. Running all tests (Single Pass).");
        print("-------------------------------------------------------------");
        int table_size = sizeof(kernel_thunk_table) / sizeof(*kernel_thunk_table);
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
}

void main(void){

    switch(pb_init()){
        case 0: break;
        default:
            XSleep(2000);
            XReboot();
            return;
    }

    pb_show_debug_screen();

    vector_init(&tests_to_run);
    load_conf_file("config.txt");
    open_output_file("kernel_tests.log");

    print("Kernel Test Suite");
    run_tests();


    vector_free(&tests_to_run);
    close_output_file();

    XSleep(10000);
    pb_kill();
}
