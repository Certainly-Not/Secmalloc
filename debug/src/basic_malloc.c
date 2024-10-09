#include <stdlib.h>
#include <stdint.h>

void *my_malloc(size_t size);
void init_secmalloc(void);
void my_free(void *ptr);

int main(){
    init_secmalloc();
    uint8_t *p = NULL;
    uint8_t *d = NULL;

    my_malloc(0x100+0x10);

    // for (size_t i = 0; i < 0x10; i++)
    // {
    //     my_free(arr[i]);
    // }
    
    
    // p = my_malloc(0x10);
    // d = my_malloc(0x10);
    // my_free(d);
    // my_free(p);
    // p = 
 
    // for (size_t k = 0; k < 10; k++)
    // {
    //     p[k] = 0x41;
    // }
    // my_free(p);

}