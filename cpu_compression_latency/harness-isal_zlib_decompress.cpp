#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cassert>
#include <cstring>
#include <immintrin.h>

#include "igzip_lib.h"
#include <zlib.h>

#define CALGARY "/lib/firmware/calgary"

char input_buffer[64 * 1024 * 1024];
char output_buffer[64 * 1024 * 1024];
char output_buffer_2[64 * 1024 * 1024];
char level_buffer[64 * 1024 * 1024];

using namespace std;
uint64_t nanos() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
}

int compare_buffers(const char *a, const char *b, int size) {
   for (int i = 0; i < size; i++) {
      if (a[i] != b[i]) {
         return i;
      }
   }
   return -1;
}

void flush_buf(char *buf, int size) {
   for (int i = 0; i < size; i+=64) {
      _mm_clflush(&buf[i]);
   }
}

void generate_list_of_buffers(char *input_buffer, int size, int num_buffers, char **buffers) {
   for (int i = 0; i < num_buffers; i++) {
      buffers[i] = (char *)malloc(size / num_buffers);
      memcpy(buffers[i], input_buffer + (i * (size / num_buffers)), size/num_buffers);
   }
}

/*
 * This function reads the calgary corpus into a buffer and then splits it into
   * num_buffers sub-buffers. It returns the number of sub-buffers created.
*/
int corpus_to_input_buffer(char **testBufs, int subBufSize ) {
   FILE *f = fopen(CALGARY, "rb");
   int size = fseek(f, 0, SEEK_END);
   fseek(f, 0, SEEK_SET);

   int *input_buffer = (int *)malloc(size);
   int orig_size = fread(input_buffer, 1, size, f);
   
   int num_buffers = (size % num_buffers) == 0 ? (size / subBufSize) : (size / subBufSize) + 1;

   char *buffer = (char *)malloc(num_buffers * subBufSize);
   memcpy(buffer, input_buffer, orig_size);
   if(orig_size < num_buffers * subBufSize) {
      memcpy(buffer + orig_size, input_buffer, num_buffers * subBufSize - orig_size);
   }

   generate_list_of_buffers(buffer, orig_size, num_buffers, testBufs);
   return num_buffers;
}


int main(int argc, char **argv) {

   FILE *f = fopen(argv[1], "rb");

   int iterations_per_run = 10;
   int size = atoi(argv[2]);
   int do_flush = atoi(argv[3]), ret;
   int orig_size = fread(input_buffer, 1, size, f);
   assert( orig_size == size );

   struct isal_zstream stream;
   isal_deflate_init(&stream);
   stream.next_in = (uint8_t *)input_buffer;
   stream.avail_in = size;
   stream.next_out = (uint8_t *)output_buffer;
   stream.avail_out = sizeof output_buffer;
   stream.end_of_stream = 1;
   isal_deflate(&stream);

   char *backup_buffer = (char *)malloc(stream.total_out);
   memcpy(backup_buffer, output_buffer, stream.total_out);

   struct inflate_state state;
   isal_inflate_init(&state);
   state.next_in = (uint8_t *)output_buffer;
   state.avail_in = stream.total_out;
   state.next_out = (uint8_t *)output_buffer_2;
   state.avail_out = sizeof output_buffer_2;
   isal_inflate(&state);

   compare_buffers(output_buffer_2, input_buffer, size);
   
   int index;
   if((index = compare_buffers(output_buffer_2, input_buffer, size)) != -1) {
      printf("Error: decompressed buffer does not match original buffer at index %d\n", index);
      return 1;
   }
   
   int runs;
   if(!do_flush)
      runs = 1000;
   else
      runs = 10;
   iterations_per_run=1;
   int discard_fst = 10;
   uint64_t latency_avg, latency_min, latency_max;
   vector<uint64_t> latencies(runs);
   for(int i=0; i<runs; i++){
      if(do_flush){
         flush_buf(output_buffer_2, sizeof output_buffer_2);
         flush_buf(output_buffer, sizeof output_buffer);
      }
      uint64_t start = nanos();
      for(int j=0; j<iterations_per_run; j++){
         state.next_in = (uint8_t *)output_buffer;
         state.avail_in = stream.total_out;
         state.next_out = (uint8_t *)output_buffer_2;
         state.avail_out = sizeof output_buffer_2;
         isal_inflate(&state);
      }
      uint64_t end = nanos();
      printf("latency,%ld\n", end-start);
      assert(compare_buffers(output_buffer_2, input_buffer, size) == -1);
      latencies[i]=((end - start)/iterations_per_run);      
   }
   uint64_t max_latency = *max_element(&(latencies[0]), &(latencies[runs-1]));
   printf("ISAL-Decompress,%d,%f,%ld\n",
   size, 
   std::accumulate(latencies.begin()+discard_fst,
      std::next(latencies.begin(),runs-discard_fst), 0) / (runs*1.0),
      max_latency); 

   
   

   latencies.clear();
   for(int i=0; i<runs; i++){
      if(do_flush){
         flush_buf(output_buffer, sizeof output_buffer);
         flush_buf(input_buffer, sizeof input_buffer);
      }
      
      uint64_t start = nanos();
      for(int j=0; j<iterations_per_run; j++){
         stream.next_in = (uint8_t *)input_buffer;
         stream.avail_in = size;
         stream.next_out = (uint8_t *)output_buffer;
         stream.avail_out = sizeof output_buffer;
         stream.end_of_stream = 1;
         isal_deflate(&stream);
      }
      uint64_t end = nanos();
      printf("latency,%ld\n", end-start);
      assert(compare_buffers(output_buffer, backup_buffer, stream.total_out) == -1);
      latencies[i] = (end - start)/iterations_per_run;      
   }
   max_latency = *max_element(&(latencies[0]), &(latencies[runs-1]));
   printf("ISAL-Compress,%d,%f,%f,%ld\n",
   size,
   size*1.0/stream.total_out,
   std::accumulate(latencies.begin()+discard_fst,
      std::next(latencies.begin(),runs-discard_fst), 0) / (runs*1.0),
      max_latency);   

   /* Zlib */
   z_stream strm;
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   ret = deflateInit(&strm, 3);
   if (ret != Z_OK)
        return ret;

   latencies.clear();
   for(int i=0; i<runs; i++){
      if(do_flush){
         flush_buf(output_buffer, sizeof output_buffer);
         flush_buf(input_buffer, sizeof input_buffer);
         _mm_mfence();
      }
      uint64_t start = nanos();
      for(int j=0; j<iterations_per_run; j++){
         strm.avail_in = size;
         strm.next_in = (Bytef *)input_buffer;
         strm.next_out = (Bytef *)output_buffer;
         strm.avail_out = sizeof output_buffer;
         deflate(&strm, Z_FINISH);
      }
      uint64_t end = nanos();
      // assert(compare_buffers(output_buffer, backup_buffer, size) == -1);
      latencies[i] = ((end - start)/iterations_per_run);    
      printf("latency,%ld\n", end-start);  
   }

   max_latency = *max_element(&(latencies[0]), &(latencies[runs-1]));
   printf("Zlib-Compress,%d,%f,%f,%ld\n",
   size,
   size*1.0/strm.total_out,
   std::accumulate(latencies.begin()+discard_fst,
      std::next(latencies.begin(),runs-discard_fst), 0) / (runs*1.0),
   max_latency);

   printf("%lu,%lu\n", *(latencies.begin()+2), *(latencies.end()-2));
   
   return 0;
}

