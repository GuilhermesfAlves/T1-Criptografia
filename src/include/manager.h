#ifndef MANAGER_H
#define MANAGER_H

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_help (const char *programName);
unsigned char *read_stdin_to_string(size_t *out_len);
unsigned char* read_file_to_string(const char *filename);
long long diff_ns(struct timespec start, struct timespec end);
#endif // MANAGER_H
