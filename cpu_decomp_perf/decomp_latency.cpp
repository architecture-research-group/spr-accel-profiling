#include <x86intrin.h>
#include <stdio.h>
#include <zlib.h>

#include "utils.h"


#define PAGE_SIZE 4096


int main(){
    Byte * buf, * cbuf;
    int ret;
    size_t size = 16*1024;

    ret = posix_memalign(&buf, PAGE_SIZE, sizeof(char) * size);
    if(ret != 0){
        printf("Failed alloc\n");
    }
    ret = posix_memalign(&cbuf, PAGE_SIZE, sizeof(char) * size);
    if(ret != 0){
        printf("Failed alloc\n");
    }

    generate(buf, size, .80); 
    generate_cbuf(cbuf, buf, size);
    index_into_cbuf()

    printf("Flushing...\n");
    for (int i=0; i<size; i+=64){
        _mm_clflush(buf + i);
    }

    printf("Compressing test buffer\n");


}