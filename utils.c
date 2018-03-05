#include <stdlib.h>
#include <string.h>
#include <pbkit/pbkit.h>

// From nxdk (lib/hal/fileio.c)
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
