#include <x86intrin.h>
#include <stdio.h>
#include <zlib.h>

#include "utils.h"


#define PAGE_SIZE 4096


int main(){
    Byte * buf, * cbuf;
    void * vbuf = (void *) buf; void *vcbuf  = (void *) cbuf;
    int ret;
    size_t size = 16*1024;

    ret = posix_memalign(&vbuf, PAGE_SIZE, sizeof(char) * size);
    if(ret != 0){
        printf("Failed alloc\n");
    }
    ret = posix_memalign(&vcbuf, PAGE_SIZE, sizeof(char) * size);
    if(ret != 0){
        printf("Failed alloc\n");
    }

    generate(vbuf, size, .80); 

    uLong len;
    generate_cbuf(cbuf, buf, size, &len);
    DISPLAY("Uncompressed:%ld,Compressed:%ld", size, len);

    uLong userOffset = 1024;
    // assert(len > userOffset)
    index_into_cbuf(cbuf, userOffset, userOffset);

    // printf("Flushing...\n");
    // for (int i=0; i<size; i+=64){
    //     _mm_clflush(buf + i);
    // }

    // printf("Compressing test buffer\n");


}