#include <pbkit/pbkit.h>
#include <hal/debug.h>
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "util/output.h"

BOOL output_video = FALSE; // NOTE: Must be set to a default of FALSE until config file is loaded and before video initialization.

static HANDLE output_filehandle = INVALID_HANDLE_VALUE;

void print(char* str, ...)
{
    va_list args;
    enum {
        buffer_size = 512,
        buffer_format_reserve = buffer_size - 2
    };
    char buffer[buffer_size];
    va_start(args, str);
    vsnprintf(buffer, buffer_format_reserve, str, args);
    va_end(args);

    /** PRINT ON DEBUGGER CONSOLE **/
    DbgPrint("%s", buffer);
    /*******************************/

    // Append newline once for calls below.
    strcat(buffer, "\n");

    /** PRINT ON HARDWARE SCREEN ***/
    if (output_video) {
        debugPrint("%s", buffer);
    }
    /*******************************/

    // Write information to logfile
    write_to_output_file(buffer, strlen(buffer));
}

void print_test_header(
    const char* func_num,
    const char* func_name)
{
    print("%s - %s: Tests Starting", func_num, func_name);
}

void print_test_footer(
    const char* func_num,
    const char* func_name,
    BOOL tests_passed)
{
    if(tests_passed) {
        print("%s - %s: All tests PASSED", func_num, func_name);
    }
    else {
        print("%s - %s: One or more tests FAILED", func_num, func_name);
    }
}

BOOL open_output_file(char* file_path)
{
    output_filehandle = CreateFile(
        file_path,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        0,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    return output_filehandle != INVALID_HANDLE_VALUE;
}

int write_to_output_file(
    void* data_to_print,
    DWORD num_bytes_to_print)
{
    DWORD bytes_written;
    BOOL ret = WriteFile(
        output_filehandle,
        data_to_print,
        num_bytes_to_print,
        &bytes_written,
        NULL
    );
    if(!ret && output_video) {
        debugPrint("ERROR: Could not write to output file\n");
    }
    if(bytes_written != num_bytes_to_print) {
        if (output_video) {
            debugPrint("ERROR: Bytes written = %lu, bytes expected to write = %lu\n",
                       bytes_written, num_bytes_to_print);
        }
        ret = 1;
    }
    return ret;
}

void close_output_file()
{
    BOOL ret = CloseHandle(output_filehandle);
    if(!ret) {
        print("ERROR: Could not close output file");
    }
}
