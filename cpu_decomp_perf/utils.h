#include <zlib.h>

/**************************************
*  Constants
**************************************/
#define MB *(1<<20)
#define BUFFERSIZE ((1 MB) - 1)
#define PROBATABLESIZE 4096
#define PRIME1   2654435761U
#define PRIME2   2246822519U

#define DISPLAY(...)         fprintf(stderr, __VA_ARGS__)

#define DISPLAYLEVEL(l, ...) if (displayLevel>=l) { DISPLAY(__VA_ARGS__); }
static int   displayLevel = 2;   /* 0 : no display;   1: errors;   2 : + result + interaction + warnings;   3 : + progression;   4 : + information */

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define assert(stmt,msg) \
    if(!stmt) { perror(msg); exit(EXIT_FAILURE); }

/* Return Pointer to len-size decompressed buffer */
static inline void * index_into_cbuf(Byte *cbuf, size_t offset, size_t len){
    int ret;
    Byte *outp_start; // sink offset bytes into this location

    z_stream dcomp_stream;
    dcomp_stream.zalloc = Z_NULL; // malloc && free for now https://www.zlib.net/manual.html
    dcomp_stream.zfree = Z_NULL;
    dcomp_stream.opaque = Z_NULL;

    dcomp_stream.next_out;
    dcomp_stream.avail_in=(offset);

    // (dcomp_stream.total_out < offset);

    ret = inflateInit2(&dcomp_stream, -8); //deflate rfc 1951
    assert(ret == Z_OK, "Failed dcomp stream init\n");

}

static void generate_cbuf(Byte *cbuf, Byte *inbuf, uLong size, uLong *totalOut){
    int ret;
    z_stream cstrm;
    cstrm.zalloc = Z_NULL;
    cstrm.zfree = Z_NULL;
    cstrm.opaque = Z_NULL; // USE MAlloc

    ret = deflateInit2(
        &cstrm,
        9,
        Z_DEFLATED,
        -12, /* 4KB Window -- match IAA */
        9,
        Z_DEFAULT_STRATEGY
    );

    assert(ret == Z_OK)

    // do{

    // }

    *totalOut=cstrm.total_out;
}


static unsigned int GEN_rand (unsigned int* seed)
{
    *seed =  ((*seed) * PRIME1) + PRIME2;
    return (*seed) >> 11;
}

static void generate(void* buffer, size_t buffSize, double p)
{
    char table[PROBATABLESIZE] = { 0 };
    int remaining = PROBATABLESIZE;
    unsigned pos = 0;
    unsigned s = 0;
    char* op = (char*) buffer;
    char* oend = op + buffSize;
    unsigned seed = 1;

    if (p==0.0) p=0.005;
    DISPLAY("Generating %u KB with P=%.2f%%\n", (unsigned)(buffSize >> 10), p*100);

    /* Build Table */
    while (remaining)
    {
        unsigned n = (unsigned)(remaining * p);
        unsigned end;
        if (!n) n=1;
        end = pos + n;
        while (pos<end) table[pos++]=(char)s;
        s++;
        remaining -= n;
    }

    /* Fill buffer */
    while (op<oend)
    {
        const unsigned r = GEN_rand(&seed) & (PROBATABLESIZE-1);
        *op++ = table[r];
    }
}

