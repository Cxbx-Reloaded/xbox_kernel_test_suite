#include "vector.h"

// Until the config.txt file can be loaded on the emulator, set emu by default
// and use the config.txt file to disable the flag on real hardware
int is_emu = 1;
vector tests_to_run;
