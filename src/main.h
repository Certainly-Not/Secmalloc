#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define _GNU_SOURCE
#include <string.h>
#include <sys/mman.h>

#define CAST_u8(x) (uint8_t*)(x)
#define CAST_u64(x) (uint64_t*)(x)
#define BASE_POOL_ALLOC 0x1000
#define BIG_ALLOC_BASE BASE_POOL_ALLOC*4096
#define TRUE 1 
#define FALSE 0

typedef struct _malloc_object malloc_object;
typedef struct _malloc_object{
    uint8_t free;
    malloc_object *flink;
    uint64_t canary; // canary in the middle of the functionning pointer: making the overflow of the canary more suitable to a crash
    malloc_object *blink;
    void *data_ptr; // data pool at the very end of the structure
    size_t size; 
} malloc_object;

void init_secmalloc(void);
malloc_object *get_last_metadata_pool();
malloc_object *search_for_data_ptr(void *ptr);
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);
void *my_calloc(size_t __nmemb, size_t __size);
void my_exit(int status );
size_t next_hexa_base(size_t size);

extern malloc_object *metadata_pool;
extern size_t metadata_pool_size;
extern size_t current_metadata_pool_size;
extern uint8_t *malloc_pool;
extern size_t current_allocated;
extern size_t alloc_offset;
extern uint8_t started;


#define RANDOM_CANARY(buffer) ({ \
    int fd = open("/dev/urandom", O_RDONLY); \
    if (fd < 0) { \
        perror("RANDOM:open"); \
        exit(EXIT_FAILURE); \
    } \
    if (read(fd, buffer, sizeof(uint64_t)) != sizeof(uint64_t)) { \
        perror("RANDOM:read"); \
        exit(EXIT_FAILURE); \
    } \
    ((uint8_t*)buffer)[0] = 0; \
    close(fd); \
})

#endif
