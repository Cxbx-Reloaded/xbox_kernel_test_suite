#include <xboxrt/debug.h>
#include <pbkit/pbkit.h>
#include <hal/xbox.h>
#include "stdio.h"
#include "output.h"
#include "func_table.h"

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

    int table_size = sizeof(kernel_thunk_table) / sizeof(*kernel_thunk_table);
    for(int k=1;k<table_size;k++){
        kernel_thunk_table[k]();
    }

    while(1){
      XSleep(2000);
    }

    pb_kill();
    XReboot();
    
}
