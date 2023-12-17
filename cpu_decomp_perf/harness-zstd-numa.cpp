#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <numa.h>
#include <cassert>
#include <immintrin.h>

#ifdef USE_LIBDEFLATE
#include "libdeflate/libdeflate.h"
#else
#include <zstd.h>
#endif

// char input_buffer[64 * 1024 * 1024];
// char output_buffer[64 * 1024 * 1024];

#define SIZE 64 * 1024 * 1024

uint64_t nano() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
}


int main(int argc, char **argv) {
      FILE *f = fopen(argv[1], "rb");
      char *file_buffer = (char *)malloc(64 * 1024 * 1024);
      assert(file_buffer != NULL);

      int iterations_per_run = atoi(argv[2]);
      int runs = atoi(argv[3]), ret, size;
      int compressed_size = fread(file_buffer, 1, SIZE, f);
      
      size_t outlen = SIZE;
      // int status = ZSTD_decompress((uint8_t*)output_buffer, outlen, (uint8_t*)file_buffer, compressed_size);

      double times[10000], bands[10000];


   for (int node=0; node<2; node++){
      char *input_buffer = (char *)numa_alloc_onnode(64 * 1024 * 1024, node);
      assert(input_buffer != NULL);
      assert(compressed_size < SIZE);

      memcpy(input_buffer, file_buffer, compressed_size);
      char *output_buffer = (char *)numa_alloc_onnode(64 * 1024 * 1024, node);
      assert(output_buffer != NULL);

      for (int j = 0; j < runs; j++) {
         for(int k=0; k<compressed_size; k+=64){
            _mm_clflush((void *)(input_buffer + k));
         }
         for(int k=0; k<outlen; k+=64){
            _mm_clflush((void *)(output_buffer + k));
         }
         
         {
            uint64_t start = nano();
            for (int i = 0; i < iterations_per_run; i++) {
               outlen = SIZE;
               size = ZSTD_decompress((uint8_t*)output_buffer, outlen, (uint8_t*)input_buffer, compressed_size);
            }
            uint64_t end = nano();
            double bb = (end-start);
            times[j] = bb/iterations_per_run;
            // fprintf(stderr,"%d\n", size);
            double band =  (double)(size*iterations_per_run) / (end-start);
            bands[j] = band;
         }
      }

      std::sort(&times[0], &times[runs]);

      double sum = std::accumulate(&times[0], &times[runs], 0);
      std::sort(&bands[0], &bands[runs]);
      printf("%d,%lf,%lf,%lf,%lf,%d,%d\n",node, (1.0 * size / compressed_size), (times[runs/2]), sum/runs, bands[runs/2],compressed_size, size);
   }
   

   
   return 0;
}