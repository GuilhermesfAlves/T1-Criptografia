#ifndef MANAGER_H
#define MANAGER_H

#include <stdio.h>
#include <stdlib.h>

void print_help (const char *programName);
unsigned char *read_stdin_to_string(size_t *out_len);
unsigned char* read_file_to_string(const char *filename);
#endif // MANAGER_H
