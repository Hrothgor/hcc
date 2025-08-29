#include "common.h"
#include "vector/vector.h"

Errno file_size(FILE *f, size_t *size)
{
    if (fseek(f, 0, SEEK_END) < 0) return errno;
    long result = ftell(f);
    if (result < 0) return errno;
    rewind(f);

    *size = (size_t)result;
    return 0;
}

Errno read_file(const char *file_path, char **content)
{
    Errno result = 0;
    FILE *fptr;

    fptr = fopen(file_path, "r");
    if (!fptr) return_defer(errno);

    size_t size;
    Errno err = file_size(fptr, &size);
    if (err) return_defer(err);

    char *buffer = (char *)malloc(size + 1);
    size_t read_bytes = fread(buffer, 1, size, fptr);
    if (ferror(fptr)) {
        if (buffer) free(buffer);
        return_defer(errno);
    }
    buffer[read_bytes] = '\0';
    *content = buffer;

defer:
    if (fptr) fclose(fptr);
    return result;
}

Errno read_sfile(const char *file_path, size_t file_path_len, char **content)
{
    Errno result = 0;
    
    char *path = (char *)malloc(file_path_len + 1);
    if (!path) return_defer(errno);
    memcpy(path, file_path, file_path_len);
    path[file_path_len] = '\0';

    Errno err = read_file(path, content);
    if (err) return_defer(err);

defer:
    free(path);
    return result;
}

int sstrcmp(const char *s1, const char *s2, size_t s1len)
{
    size_t s2len = strlen(s2);
    if (s1len < s2len) return -1;
    if (s1len > s2len) return 1;

    return strncmp(s1, s2, s1len);
}


vec_define(PtrVector, void *);
static PtrVector garbage_collector = {0};

void garbage_collect(void *ptr)
{
    if (!ptr) return;

    vec_push_back(&garbage_collector, ptr);
}

void garbage_release()
{
    for (size_t i = 0; i < vec_size(&garbage_collector); i++) {
        free(vec_at(&garbage_collector, i));
    }
    vec_free(&garbage_collector);
}