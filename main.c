#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/xbox.h>
#include "stdio.h"
#include "output.h"
#include "global.h"
#include "func_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *getCurrentDirString()
{
	char *tmp;
    char *currentDirString = NULL;

    currentDirString = malloc(XeImageFileName->Length + 1);
	strcpy(currentDirString, XeImageFileName->Buffer);
	// Remove XBE name, leaving the path
	tmp = strrchr(currentDirString, '\\');
	if (tmp) {
		*(tmp + 1) = '\0';
	} else {
		free(currentDirString);
		currentDirString = NULL;
	}
	return currentDirString;
}

int open_conf_file(){
    /*HANDLE handle;
    NTSTATUS status;
    OBJECT_ATTRIBUTES obj;
    IO_STATUS_BLOCK isb;
    ANSI_STRING obj_name;
    char filepath[200];


    ULONG uSize = 200;
    char readed[200];
    strcpy(filepath, getCurrentDirString());
    strcat(filepath, "config.txt");
    print(filepath);

    RtlInitAnsiString(&obj_name, filepath);

    obj.RootDirectory = NULL;
    obj.Attributes =  OBJ_CASE_INSENSITIVE;
    obj.ObjectName = &obj_name;

    status = NtCreateFile(
   		&handle,
        GENERIC_READ,
   		&obj,
   		&isb,
   		NULL,
   		FILE_ATTRIBUTE_NORMAL,
   		0,
   		FILE_OPEN,
   		FILE_SYNCHRONOUS_IO_NONALERT);

    if (!NT_SUCCESS(status)) {
        print("Error opening config file: %s not found", filepath);
        NtClose(handle);
        return -1;
    }

    status = NtReadFile(
	    handle,
		NULL,
		NULL,
		NULL,
		&isb,
		readed,
		uSize,
		NULL);

    if (status == STATUS_PENDING)
		status = NtWaitForSingleObject((void*)handle, FALSE, (void*)NULL);

    if (!NT_SUCCESS(status)) {
        print("Error reading config file");
        NtClose(handle);
        return -1;
    }

    NtClose(handle);
    print(readed);

    char *token;
    char *saveptr;
    token = strtok_r(readed, ",", &saveptr);
    while(token != NULL){
        print("CALL TO TEST: %d", atoi(token));
        token = strtok_r(readed, ",", &saveptr);
    }*/

    return 0;
}

void main(void)
{

    switch(pb_init())
    {
        case 0: break;
        default:
            XSleep(2000);
            XReboot();
            return;
    }

    pb_show_debug_screen();

    print("Kernel Test Suite");

    if(open_conf_file()==0){ // FIXME this check is currently WIP
        print("Config File Loaded");
    }
    else{
        print("Config File Not Found");
        print("Testing Everything (Single Pass)");
    }

	is_real_hw = 0; // FIXME check if we are using an emu or real hw

	int table_size = sizeof(kernel_thunk_table) / sizeof(*kernel_thunk_table);
	for(int k=1;k<table_size;k++){
		kernel_thunk_table[k]();
	}

    while(1){
        XSleep(10000);
        pb_kill();
        return;
    }

    pb_kill();
    XReboot();

}
