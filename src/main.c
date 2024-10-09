#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "main.h"
#include "log.h"

#ifndef _DYNAMIC
#define _DYNAMIC FALSE
#endif
malloc_object *metadata_pool;
size_t metadata_pool_size;
size_t current_metadata_pool_size;
uint8_t *malloc_pool;
size_t current_allocated;
size_t alloc_offset;
uint8_t started;

#if _DYNAMIC == TRUE
void *malloc(size_t size){
    return my_malloc(size);
}
void free(void *ptr){
    my_free(ptr);
}
void *realloc(void *ptr, size_t size){
    return my_realloc(ptr, size);
}
void *calloc(size_t __nmemb, size_t __size){
    return my_calloc(__nmemb, __size);
}

void exit(int status ){
    my_exit(status);
}

#endif

void my_exit(int status ){
    malloc_object *p = metadata_pool;
    while (p->flink != NULL) {
        if (p->free != TRUE)
        {
            my_log(LOG_WARNING, "my_exit: ptr %p from metadata %p not free at the end of the function\n", p->data_ptr, p);
            free(p->data_ptr);
            my_log(LOG_WARNING, "my_exit: %p is now free.\n", p->data_ptr);
        }
        p = p->flink;
    }
    __asm__("push %rdi");
    if(munmap(metadata_pool, BIG_ALLOC_BASE) != 0){
        my_log(LOG_WARNING, "my_exit: Failed to munmap the METADATA pool\n");
        exit(EXIT_FAILURE);
    }
    if(munmap(malloc_pool, BIG_ALLOC_BASE) != 0){
        my_log(LOG_WARNING, "my_exit: Failed to munmap the DATA pool\n");
        exit(EXIT_FAILURE);
    }
    __asm__("pop %rdi");
    started = FALSE;
    __asm__("mov $60, %rax\n\t"
            "syscall");

}

void *my_calloc(size_t __nmemb, size_t __size){
    my_log(LOG_INFO, "Entering calloc(%llx, %llx)\n", __nmemb, __size);
    void *p = my_malloc(__nmemb*__size);
    my_log(LOG_INFO, "Exiting calloc %p\n", p);
    return p;
}

malloc_object *get_last_metadata_pool() {
    malloc_object *p = metadata_pool;
    while (p->flink != NULL) {
        p = p->flink;
    }
    return p;
}
uint8_t search_for_freed_data(void *ptr) {
    malloc_object *p = metadata_pool;
    while (p->flink != NULL) {
        if(p->free && (CAST_u64(ptr)>=CAST_u64(p->data_ptr) && CAST_u64(ptr)<CAST_u64(p->data_ptr+p->size))){
            return TRUE;
        }
        p = p->flink;
    
    }
    return FALSE;
}
malloc_object *search_min_free_in_pool(size_t size){
    size_t best_choice = 0;
    malloc_object *result = NULL;
    malloc_object *p = metadata_pool;
    while (p->flink != NULL) {
        if(p->free && best_choice < p->size && p->size>=size && p->size){
            result = p;
            best_choice = p->size;
        }
        p = p->flink;
    }
    return result;
}

malloc_object *search_for_data_ptr(void *ptr) {
    my_log(LOG_INFO, "Entering search_for_data_ptr\n");
    malloc_object *p = metadata_pool;
    while (p->flink != NULL) {
        if(p->data_ptr == ptr){
            return p;
        }
        p = p->flink;
    }
    my_log(LOG_INFO, "Exiting search_for_data_ptr\n");
    return NULL;
}
size_t next_hexa_base(size_t size)
{
    return (size % 16 ? size + 16 - (size % 16) : size);
}


void *my_malloc(size_t size)
{
    size = next_hexa_base(size);
    malloc_object *current_block = { 0 };

    if(started == FALSE){
        init_secmalloc();
        started = TRUE;
    }

    my_log(LOG_INFO, "Starting malloc 0x%llx\n", size);

    current_block = search_min_free_in_pool(size);
    if(current_block != NULL){
        current_block->free = FALSE;
        return current_block->data_ptr;
    }
    current_block = get_last_metadata_pool();
    if(alloc_offset+size+sizeof(uint64_t) >= current_allocated){
        
        while (alloc_offset+size+sizeof(uint64_t) >= current_allocated)
        {
            // malloc_pool = mremap(malloc_pool, current_allocated, current_allocated+BASE_POOL_ALLOC, MREMAP_FIXED);
            // if(malloc_pool == MAP_FAILED){
            //     perror("malloc:mremap failed");
            //     exit(EXIT_FAILURE);
            // }
            if(current_allocated >=BIG_ALLOC_BASE){
                my_log(LOG_ERROR, "malloc: Max Data Pool Allocated triggered ! Exiting...\n");
                exit(EXIT_FAILURE);
            }
            current_allocated+=BASE_POOL_ALLOC;
        }
    }

    RANDOM_CANARY(&current_block->canary);
    current_block->data_ptr = malloc_pool+alloc_offset;
    memcpy(current_block->data_ptr+size, &current_block->canary, sizeof(uint64_t));
    alloc_offset+=size+sizeof(uint64_t);

    if((CAST_u8(current_block)+sizeof(malloc_object))-CAST_u8(metadata_pool)>current_metadata_pool_size){
        current_metadata_pool_size+=sizeof(malloc_object);
        if(current_metadata_pool_size>metadata_pool_size-sizeof(malloc_object)){
            // metadata_pool = mremap(metadata_pool, metadata_pool_size, metadata_pool_size+BASE_POOL_ALLOC, MREMAP_MAYMOVE);
            // if(metadata_pool == MAP_FAILED){
            //     perror("malloc:mremap failed");
            //     exit(EXIT_FAILURE);
            // }
            if(metadata_pool_size>=BIG_ALLOC_BASE){
                my_log(LOG_ERROR, "malloc: Max Metadata Pool Allocated triggered ! Exiting...\n");
                exit(EXIT_FAILURE);
            }
            metadata_pool_size+=BASE_POOL_ALLOC;
        }
    }


    current_block->size = size;

    current_block->flink=(malloc_object*)(CAST_u8(current_block)+sizeof(malloc_object));
    current_block->flink->flink = NULL;
    current_block->flink->blink = current_block;
    current_block->free = FALSE;

    my_log(LOG_INFO, "Exiting malloc %p\n", current_block->data_ptr);
    return current_block->data_ptr;
}

void my_free(void *ptr){
    my_log(LOG_INFO, "Entering free %p\n", ptr);
    if(ptr == NULL){
        return;
    }
    malloc_object *p = search_for_data_ptr(ptr);
    malloc_object *search_free_backward = { 0 };
    malloc_object *search_free_forward = { 0 }; 
    size_t new_size = 0;
    size_t free_chunk_count = 0;    
    
    if(search_for_freed_data(ptr)){
        my_log(LOG_ERROR, "free: Double free detected !!! Exiting...\n");
        exit(EXIT_FAILURE);
    }
    if(p == NULL){
        my_log(LOG_WARNING, "free: search_for_data_ptr ptr not found returning...\n");
        return;
    }
    if(p->canary != *CAST_u64((ptr+p->size))){
        my_log(LOG_ERROR, "free: Heap Smashing detected !!! Exiting...\n");
        exit(EXIT_FAILURE);
    }

    p->free = TRUE;

    search_free_forward = p;
    while (search_free_forward->flink != NULL && search_free_forward->flink->free == TRUE) {
        search_free_forward = search_free_forward->flink;
        free_chunk_count += 1;
    }

    search_free_backward = p;
    while (search_free_backward->blink != NULL && search_free_backward->blink->free == TRUE) {
        search_free_backward = search_free_backward->blink;
        free_chunk_count += 1;
    }
    
    new_size = -sizeof(uint64_t);
    p = search_free_backward;
    while (p != NULL && p->free == TRUE) {
        new_size += p->size + sizeof(uint64_t);
        p = p->flink;
    }



    search_free_backward->flink = search_free_forward->flink;
    // search_free_forward->blink = search_free_backward->blink;

    search_free_forward->flink->blink = search_free_backward;


    search_free_backward->size = new_size;

    memset(search_free_backward->data_ptr, 0x0, search_free_backward->size); // UAF mitigation
    // memset(search_free_backward->data_ptr,0xab,search_free_backward->size );
    memcpy(search_free_backward->data_ptr+search_free_backward->size, &search_free_backward->canary, sizeof(uint64_t));
    my_log(LOG_INFO, "Exiting free\n");
}


void *my_realloc(void *ptr, size_t size){
    my_log(LOG_INFO, "Entering realloc(%p, %llx)\n", ptr, size);
    if(ptr == NULL){
        my_log(LOG_INFO, "realloc: ptr NULL\n");
        return my_malloc(size);
    }
    if(size == 0){
        my_log(LOG_INFO, "realloc: size == 0\n");
        my_free(ptr);
        return NULL;
    }
    void *p = my_malloc(size);
    memcpy(p,ptr,size);
    my_free(ptr);
    my_log(LOG_INFO, "Exiting realloc\n");
    return p;
}

void init_secmalloc(void)
{
    my_log(LOG_INFO, "Starting secmalloc\n");
    metadata_pool = mmap(NULL, BIG_ALLOC_BASE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (metadata_pool == MAP_FAILED) {
        my_log(LOG_ERROR, "init_secmalloc mmap failed\n");
        exit(EXIT_FAILURE);
    }
    metadata_pool_size+=BASE_POOL_ALLOC;
    
    metadata_pool->blink = NULL;
    metadata_pool->flink = NULL;
    metadata_pool->free = FALSE;

    malloc_pool = mmap(NULL, BIG_ALLOC_BASE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (metadata_pool == MAP_FAILED) {
        my_log(LOG_ERROR, "init_secmalloc mmap failed\n");
        exit(EXIT_FAILURE);
    }
    current_allocated +=BASE_POOL_ALLOC;
    my_log(LOG_INFO, "Exiting secmalloc\n");
}

// set environment LD_PRELOAD /home/prince2lu/Desktop/cours/secmalloc/build/secmalloc.so
