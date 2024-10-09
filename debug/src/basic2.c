#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
int main(){
    // init_secmalloc();
    uint8_t *p = NULL;

    p = malloc(0x10);
    memcpy(p, "Hello World !", 15);
    printf("%s\n",p);
    free(p);

    // my_free(p);
    
    // p = 
 
    // for (size_t k = 0; k < 10; k++)
    // {
    //     p[k] = 0x41;
    // }
    // my_free(p);

}