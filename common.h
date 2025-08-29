#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UNUSED(value) (void)(value)
#define TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define shift(xs, size) ((size)--, *(xs)++)

#define return_defer(value) do { result = (value); goto defer; } while(0)

typedef int Errno;

Errno file_size(FILE *f, size_t *size);
Errno read_file(const char *file_path, char **content);
Errno read_sfile(const char *file_path, size_t file_path_len, char **content);

int sstrcmp(const char *s1, const char *s2, size_t s1len);

void garbage_collect(void *ptr);
void garbage_release();

#endif // COMMON_H_