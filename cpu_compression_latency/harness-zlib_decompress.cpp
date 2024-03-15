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

#include <zlib.h>

#include "timer.h"
#define CALGARY "./calgary"

// char input_buffer[64 * 1024 * 1024];
// char output_buffer[64 * 1024 * 1024];
// char output_buffer_2[64 * 1024 * 1024];
char level_buffer[64 * 1024 * 1024];

using namespace std;

int main(int argc, char **argv) {


   int size = atoi(argv[1]);
   int num_iters = atoi(argv[2]);

   assert( argc >= 3);
   char *file;
   char **input_buffers;

   int num_bufs;
   if(argc >= 4){
      file = argv[3];
  num_bufs = corpus_to_input_buffer(input_buffers, size, file);
   }
   else{
  num_bufs = calgary_corpus_to_input_buffer(input_buffers, size);
   }

   char **output_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   int compressed_sizes[num_bufs];
   char **output_buffers_2 = (char **)malloc(sizeof(char *) * num_bufs);
   char **backup_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   for(int i=0; i<num_bufs; i++){
      output_buffers[i] = (char *)malloc(size + 1024); /* What is max expansion of zlib deflated buf?*/
      output_buffers_2[i] = (char *)malloc(size);
   }

   printf("Testing on %d buffers\n", num_bufs);
   num_iters = num_iters > num_bufs ? num_iters : num_bufs; //max(num_bufs, num_iters);
   printf("Testing on %d iterations\n", num_iters);
   /* DEFLATE */
   z_stream stream;
   int level = 9;
   stream.zalloc = Z_NULL;
   stream.zfree = Z_NULL;
   stream.opaque = Z_NULL;
   deflateInit(&stream, level);

   vector <uint64_t> times(num_iters);
   for(int i=0; i<num_iters; i++){
      deflateInit(&stream, level);
      stream.next_in = (uint8_t *)input_buffers[i % num_bufs];
      stream.avail_in = size;
      stream.next_out = (uint8_t *)output_buffers[i % num_bufs];
      stream.avail_out = size + 1024;
      int status;
      uint64_t start = nanos();
      do{
         status = deflate(&stream, Z_FINISH);
      } while(stream.avail_out == 0);
      if( status != Z_STREAM_END){
         printf("Error: Compression failed: %d\n", status);
         return -1;
      }
      uint64_t end = nanos();
      times[i] = end - start;
      compressed_sizes[i] = stream.total_out;
   }
   printf("Size,Level,AvgRatio,Direction,Latency\n");
   int compressed_sum = 0;
   for(auto& compressed_size : compressed_sizes)
      compressed_sum += compressed_size;
   double avg_ratio = compressed_sum /
      (1.0 * size * num_bufs)
         ;
   double max_time = *max_element(times.begin(), times.end());
   double avg_time = accumulate(begin(times),end(times),0) / num_iters;
   printf("%d,%d,%f,%s,%f,%f\n", size, level, avg_ratio, "Compress", avg_time, max_time);

   z_stream state;
   for(int i=0; i<num_iters; i++){
      assert( Z_OK == inflateInit(&state));
      state.next_in = (uint8_t*)(output_buffers[i % num_bufs]);
      state.avail_in = compressed_sizes[i % num_bufs];
      state.next_out = (uint8_t*)output_buffers_2[i % num_bufs];
      state.avail_out = size;
      uint64_t start = nanos();
      int status = inflate(&state, Z_FINISH);
      uint64_t end = nanos();
      if(status != Z_STREAM_END){
         printf("Error: Decompression failed: %d\n", status);
         return -1;
      }
      times[i] = end - start;
      int mismatch = compare_buffers(output_buffers_2[i % num_bufs], input_buffers[i % num_bufs], size);
      if(mismatch != -1){
         printf("Error: Decompression mismatch buffer:%d index:%d \n", 0, mismatch );
         return -1;
      }

   }
   max_time = *max_element(times.begin(), times.end());
   avg_time = accumulate(begin(times),end(times),0) / num_iters;
   printf("%d,%d,%f,%s,%f,%f\n", size, level, avg_ratio, "Decompress", avg_time, max_time);
   return 0;


}

