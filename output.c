#include <pbkit/pbkit.h>
#include <hal/debug.h>
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "output.h"

static HANDLE output_filehandle = INVALID_HANDLE_VALUE;

void print(char* str, ...){
    va_list args;
    char buffer[500];
    va_start (args, str);
    vsnprintf (buffer, 499, str, args);
    va_end(args);
    /**** PRINT ON TV (REAL HW) ****/
    debugPrint(buffer);
    debugPrint("\n");
    /*******************************/

    /*** PRINT ON CONSOLE (CXBX) ***/
    DbgPrint("%s", buffer);
    /*******************************/

    // Write information to logfile
    strcat(buffer, "\n");
    write_to_output_file(
        buffer,
        strlen(buffer)
    );
}

void print_test_header(const char* func_num, const char* func_name) {
    print("%s - %s: Tests Starting", func_num, func_name);
}

void print_test_footer(
    const char* func_num, const char* func_name, BOOL tests_passed
) {
    if(tests_passed) {
        print("%s - %s: All tests PASSED", func_num, func_name);
    }
    else {
        print("%s - %s: One or more tests FAILED", func_num, func_name);
    }
}

void open_output_file(char* file_name) {
    if(is_emu) {
        print("Kernel Test Suite: Skipping creating %s because on emulator", file_name);
        return;
    }

    debugPrint("Creating file %s", file_name);
    output_filehandle = CreateFile(
        file_name,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        0,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if(output_filehandle == INVALID_HANDLE_VALUE) {
        debugPrint("ERROR: Could not create file %s", file_name);
    }
}

int write_to_output_file(void* data_to_print, DWORD num_bytes_to_print) {
    if(is_emu) {
        return 0;
    }

    DWORD bytes_written;
    BOOL ret = WriteFile(
        output_filehandle,
        data_to_print,
        num_bytes_to_print,
        &bytes_written,
        NULL
    );
    if(!ret) {
        debugPrint("ERROR: Could not write to output file");
    }
    if(bytes_written != num_bytes_to_print) {
        debugPrint("ERROR: Bytes written = %u, bytes expected to write = %u",
                   bytes_written, num_bytes_to_print);
        ret = 1;
    }
    return ret;
}

BOOL close_output_file() {
    if(is_emu) {
        return 0;
    }
    BOOL ret = CloseHandle(output_filehandle);
    if(!ret) {
        debugPrint("ERROR: Could not close output file");
    }
    return ret;
}
