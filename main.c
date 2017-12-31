#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/xbox.h>
#include <hal/fileio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "output.h"
#include "global.h"
#include "func_table.h"
#include "vector.h"

int load_conf_file() {
    char file_path[200];
    int handle;
    unsigned int file_size = 0;
    char *buffer;
    unsigned int read = 0;
    int result;

    strcpy(file_path, getCurrentDirString());
    strcat(file_path, "config.txt");
    print(file_path);

    result = XCreateFile(&handle,
        file_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL);
    if(result != 0)
        return -1;

    XGetFileSize(handle, &file_size);

    buffer = malloc(file_size);
    if(buffer == NULL)
        return -1;

    result = XReadFile(handle, buffer, file_size, &read);
    if(result == 0 || read != file_size)
        return -1;

    XCloseHandle(handle);

    vector_init(&tests_to_run);

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

void main(void){

    switch(pb_init()){
        case 0: break;
        default:
            XSleep(2000);
            XReboot();
            return;
    }

    pb_show_debug_screen();

    print("Kernel Test Suite");

    if(load_conf_file() == 0){
        print("Config File Loaded");
        print("is_emu: %d", is_emu);
        print("---------------");
        for(int k=0; k<tests_to_run.size; k++){
            kernel_thunk_table[vector_get(&tests_to_run, k)]();
        }
    }
    else{
        print("Config File Not Found");
        print("Testing Everything (Single Pass And Assuming Running on Emulator)");
        print("---------------");
        int table_size = sizeof(kernel_thunk_table) / sizeof(*kernel_thunk_table);
    	for(int k=0;k<table_size;k++){
    		kernel_thunk_table[k]();
    	}
    }

    XSleep(10000);

    vector_free(&tests_to_run);

    pb_kill();
    XReboot();

}
