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

char level_buffer[64 * 1024 * 1024];
#include "timer.h"

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
	   printf("Corpus file:%s\n", file);
   }
   else{
	   num_bufs = calgary_corpus_to_input_buffer(input_buffers, size);
   }
   char **output_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   int compressed_sizes[num_bufs];
   char **output_buffers_2 = (char **)malloc(sizeof(char *) * num_bufs);
   char **backup_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   for(int i=0; i<num_bufs; i++){
      output_buffers[i] = (char *)malloc(MAX_EXPAND_ISAL(size));
      output_buffers_2[i] = (char *)malloc(size);
   }

   num_iters = num_iters > num_bufs ? num_iters : num_bufs;
   /* DEFLATE */
   int level = 2;
   struct isal_zstream stream;
   stream.level = level;

   vector <uint64_t> times(num_iters);
   for(int i=0; i<num_iters; i++){
      isal_deflate_init(&stream);
      stream.end_of_stream = 1;
      stream.next_in = (uint8_t *)input_buffers[i % num_bufs];
      stream.avail_in = size;
      stream.next_out = (uint8_t *)output_buffers[i % num_bufs];
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
   printf("Size,Level,AvgRatio,Direction,AvgLatency,MaxLatency\n");
   int compressed_sum = 0;
   for(auto& compressed_size : compressed_sizes)
      compressed_sum += compressed_size;
   double avg_ratio = compressed_sum /
      (1.0 * size * num_bufs) ;
   double max_time = *max_element(times.begin(), times.end());
   double avg_time = accumulate(begin(times),end(times),0) / num_iters;
   printf("%d,%d,%f,%s,%f,%f\n", size, level, avg_ratio, "Compress", avg_time, max_time);

   for(int i=0; i<num_iters; i++){
      struct inflate_state state;
      isal_inflate_init(&state);
      state.next_in = (uint8_t*)(output_buffers[i % num_bufs]);
      state.avail_in = compressed_sizes[i % num_bufs];
      state.next_out = (uint8_t*)output_buffers_2[i % num_bufs];
      state.avail_out = size;
      uint64_t start = nanos();
      int status = isal_inflate(&state);
      uint64_t end = nanos();
      if(status != ISAL_DECOMP_OK){
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

