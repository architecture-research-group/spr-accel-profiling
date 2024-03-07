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
#include "timer.h"
#include <zlib.h>

#define CALGARY "./calgary"

/*
(base) n869p538@sapphire:cpu_compression_latency$ parallel taskset -c {} ./harness-isal_zlib_throughput /lib/firmware/calgary 1 100 ::: 20 21 22 23
33520546.391753
33520546.391753
33178500.000000
33520546.391753
*/

char input_buffer[64 * 1024 * 1024];
char output_buffer[64 * 1024 * 1024];

using namespace std;
int main(int argc, char **argv) {

   int size = atoi(argv[1]);
   int num_iters = atoi(argv[2]);

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
   num_iters = num_iters > num_bufs ? num_iters : num_bufs; //max(num_bufs, num_iters);
   printf("Testing on %d iterations\n", num_iters);
   /* DEFLATE */
   struct isal_zstream stream;

   size_t outlen = sizeof output_buffer;
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
      double bb = (end-start);
      times[i] = bb;
      compressed_sizes[i] = stream.total_out;

   }

   printf("Size,AvgRatio,Direction,Bandwidth(Mbit/s)\n");
   vector<double> bandwidths(num_iters);
   for(int i=0; i<num_iters; i++){
      bandwidths[i] = (size*8) / (times[i] / 1000.0);
   }
   int compressed_sum = 0;
   for(auto& compressed_size : compressed_sizes)
      compressed_sum += compressed_size;
   double avg_ratio = 
      (1.0 * size * num_bufs) / 
         compressed_sum;
   double max_bandwidth = *max_element(bandwidths.begin(), bandwidths.end());
   double avg_bandwidths = accumulate(begin(bandwidths),end(bandwidths),0) / num_iters;
   printf("%d,%f,%s,%f,%f\n", size, avg_ratio, "Compress", avg_bandwidths, max_bandwidth);


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
      double bb = (end-start);
      times[i] = bb;
      int mismatch = compare_buffers(output_buffers_2[i % num_bufs], input_buffers[i % num_bufs], size);
      if(mismatch != -1){
         printf("Error: Decompression mismatch buffer:%d index:%d \n", 0, mismatch );
         return -1;
      }
   }
   for(int i=0; i<num_iters; i++){
      bandwidths[i] = (size*8) / (times[i] / 1000.0);
   }
   max_bandwidth = *max_element(bandwidths.begin(), bandwidths.end());
   avg_bandwidths = accumulate(begin(bandwidths),end(bandwidths),0) / num_iters;
   // printf("bw:%f,sum_bw:%d,time:%ld\n", bandwidths[num_iters-1],accumulate(begin(bandwidths),end(bandwidths),0),times[num_iters-1]);
   printf("%d,%f,%s,%f,%f\n", size, avg_ratio, "Decompress", avg_bandwidths, max_bandwidth);

   
   return 0;
}