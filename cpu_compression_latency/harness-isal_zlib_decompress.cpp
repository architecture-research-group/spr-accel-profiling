#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cassert>
#include <cstring>
#include <immintrin.h>
#include <fstream>
#include <vector>

#include "igzip_lib.h"
#include <zlib.h>

#define CALGARY "./calgary"

// char input_buffer[64 * 1024 * 1024];
// char output_buffer[64 * 1024 * 1024];
// char output_buffer_2[64 * 1024 * 1024];
char level_buffer[64 * 1024 * 1024];

using namespace std;
uint64_t nanos() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
}

uint64_t micros() {
   return std::chrono::duration_cast< ::std::chrono::microseconds>(
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

/*
 * This function reads the calgary corpus into a buffer and then splits it into
   * num_buffers sub-buffers. It returns the number of sub-buffers created.
*/
int corpus_to_input_buffer(char ** &testBufs,int sizePerBuf ) {
   FILE *file = fopen(CALGARY, "rb");
   fseek(file, 0, SEEK_END);
   int size = ftell(file);
   fseek(file, 0, SEEK_SET);
   int max_bufs = size / sizePerBuf;
   testBufs = (char **)malloc(sizeof(char *) * max_bufs);
   std::ifstream infile(CALGARY, std::ios::binary);
   int i=0;
   if (file) {
      while (true) {
         testBufs[i] = (char *)malloc(sizePerBuf);
         if (!infile.read(testBufs[i], sizePerBuf)) { /*fail if not enough content*/
               break;
         }
         i++;
      }
   }
   return i;
}
#define MAX_EXPAND_ISAL(size) ISAL_DEF_MAX_HDR_SIZE + size

int main(int argc, char **argv) {


   int size = atoi(argv[1]);
   int do_flush = atoi(argv[2]), ret;
   int iterations_per_run = atoi(argv[3]);
   int discard_fst = atoi(argv[4]);


   char **input_buffers;
   int num_bufs = corpus_to_input_buffer(input_buffers, size);
   char **output_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   int compressed_sizes[num_bufs];
   char **output_buffers_2 = (char **)malloc(sizeof(char *) * num_bufs);
   char **backup_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   for(int i=0; i<num_bufs; i++){
      output_buffers[i] = (char *)malloc(MAX_EXPAND_ISAL(size));
      output_buffers_2[i] = (char *)malloc(size);
   }

   printf("Testing on %d buffers\n", num_bufs);

   /* DEFLATE */
   struct isal_zstream stream;
   
   vector <uint64_t> times(num_bufs);
   for(int i=0; i<num_bufs; i++){
      isal_deflate_init(&stream);
      stream.end_of_stream = 1;
      stream.next_in = (uint8_t *)input_buffers[i];
      stream.avail_in = size;
      stream.next_out = (uint8_t *)output_buffers[i];
      stream.avail_out = MAX_EXPAND_ISAL(size);
      uint64_t start = nanos();
      do{
         int status = isal_deflate(&stream);
         if( status != COMP_OK){
            printf("Error: Compression failed\n");
            return -1;
         }
      } while(stream.avail_out == 0);
      uint64_t end = nanos();
      times[i] = end - start;
      compressed_sizes[i] = stream.total_out;
   }
   printf("Size,AvgRatio,Direction,Latency\n");
   int compressed_sum = 0;
   for(auto& compressed_size : compressed_sizes)
      compressed_sum += compressed_size;
   double avg_ratio = 
      (1.0 * size * num_bufs) / 
         compressed_sum;
   double avg_time = accumulate(begin(times),end(times),0) / num_bufs;
   printf("%d,%f,%s,%f\n", size, avg_ratio, "Compress", avg_time);
   
   for(int i=0; i<num_bufs; i++){
      struct inflate_state state;
      isal_inflate_init(&state);
      state.next_in = (uint8_t*)(output_buffers[i]);
      state.avail_in = compressed_sizes[i];
      state.next_out = (uint8_t*)output_buffers_2[i];
      state.avail_out = size;
      int status = isal_inflate(&state);
      if(status != ISAL_DECOMP_OK){
         printf("Error: Decompression failed: %d\n", status);
         return -1;
      }
      int mismatch = compare_buffers(output_buffers_2[i], input_buffers[i], size);
      if(mismatch != -1){
         printf("Error: Decompression mismatch buffer:%d index:%d \n", 0, mismatch );
         return -1;
      }
   }
   return 0;


}

