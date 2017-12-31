#ifndef UTILS_H
#define UTILS_H

char * strtok( char * s1, const char * s2 );
char* strtok_r(char *str, const char *delim, char **nextp);
char *getCurrentDirString();
long strtol(const char *nptr, char **endptr, register int base);

#endif
