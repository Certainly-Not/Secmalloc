#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
void *my_malloc(size_t size);
void init_secmalloc(void);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);

int main(){
    uint8_t *a, *b, *c, *d = NULL;
    a = my_malloc(0x10);
    b = my_malloc(0x20);
    c = my_malloc(0x30);
    my_free(a);
    my_free(b);
    // my_free(c);
    d = my_malloc(0x70);
    memset(d, 0x90, 0x70);
    // free(d);
    d = realloc(d,0x200);
    memset(d, 0x90, 0x201);
    my_free(d);
    return 1;
}