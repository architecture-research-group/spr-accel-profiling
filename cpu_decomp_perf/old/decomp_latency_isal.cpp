#include <x86intrin.h>
#include <stdio.h>
#include <zlib.h>

#include "utils.h"
#include "igzip_lib.h"


//

static inline int do_isal_deflate(void *buf, void *cbuf, size_t size, size_t *olen){
    struct isal_zstream stream;

    /* Initial Context Setup */

    isal_deflate_init(&stream);

    stream.end_of_stream = 1;
	stream.flush = NO_FLUSH;

    stream.avail_out = size;
    stream.next_in=buf;
    stream.avail_in=size;

    stream.next_out = cbuf;
    isal_deflate(&stream); // huffman only isa-l?
    assert(stream.avail_in == 0, "isa-l-deflate fail\n");
    assert(stream.internal_state.state == ZSTATE_END, "isa-l-deflate fail\n");
    *olen = (size-stream.avail_out);

    DISPLAY("uncompressed:%ld,compressed:%ld\n",size,(size-stream.avail_out));

}

// int isal_inflate(void *buf)

int zlib_deflate(void *inbuf, void *cbuf, size_t size){
    int ret;
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL; // USE MAlloc

    ret = deflateInit2(
        &strm,
        9,
        Z_DEFLATED,
        -12, /* 4KB Window -- match IAA */
        9,
        Z_HUFFMAN_ONLY
    );
    

    strm.next_in=inbuf;
    strm.avail_in=size;
    strm.avail_out = size;
    strm.next_out=cbuf;

    ret = deflate(&strm, Z_NO_FLUSH);
    assert(ret == Z_OK, "Failed zlib deflate-noflush\n");
    DISPLAY("avail_in:%d,avail_out:%d\n",strm.avail_in,strm.avail_out);
    ret = deflate(&strm, Z_SYNC_FLUSH);
    assert(ret == Z_OK, "Failed zlib deflate-finish\n");
    DISPLAY("avail_in:%d,avail_out:%d\n",strm.avail_in,strm.avail_out);
    DISPLAY("uncompressed:%ld,compressed:%ld\n",size,size - strm.avail_out);
}

int main(){
    void * buf, * cbuf, *obuf;
    int ret;
    void *vbuf=(void *)buf;
    size_t size = 16*1024, len;

    ret = posix_memalign(&buf, PAGE_SIZE, sizeof(char) * size); 
    assert(ret==0,"Failed alloc\n");
    ret = posix_memalign(&cbuf, PAGE_SIZE, sizeof(char) * size);
    assert(ret==0,"Failed alloc\n");
    ret = posix_memalign(&obuf, PAGE_SIZE, sizeof(char) * size);
    assert(ret==0,"Failed alloc\n");


    genRandomData(buf, size); 

    FILE *out;
    out = fopen("rand.txt","wb");
    fwrite(buf,1,size,out);
    fclose(out);

    size_t clen, olen;
    do_isal_deflate(buf,cbuf,size, &clen);
    
    

    /* */
    uint64_t st, end;

    struct inflate_state state;
    isal_inflate_init(&state);

    st = CurrentTime_nanoseconds();
    state.next_in = cbuf;
	state.avail_in = clen;
	state.next_out = obuf;
	state.avail_out = size;
    ret = isal_inflate(&state);
    end = CurrentTime_nanoseconds();

    assert(ret == ISAL_DECOMP_OK, "isa-l inflate fail\n");
    DISPLAY("ISA-L inflate: %ld\n", end - st);

    for(int i=0; i<size; i+=64){
        _mm_clflush( (void *)(&  (static_cast<char *>(cbuf)[i])   )  );
    }
    st = CurrentTime_nanoseconds();
    state.next_in = cbuf;
	state.avail_in = clen;
	state.next_out = obuf;
	state.avail_out = size;
    ret = isal_inflate(&state);
    end = CurrentTime_nanoseconds();
    DISPLAY("ISA-L inflate flushed: %ld\n", end - st);

    // state.next_in = in_buf;
	// state.avail_in = DOUBLE_SYM_THRESH + 1;
	// state.next_out = tmp_space;
	// state.avail_out = sizeof(tmp_space);

    // isal_inflate(cbuf, obuf, clen, olen);

    zlib_deflate(buf,cbuf,size);


}