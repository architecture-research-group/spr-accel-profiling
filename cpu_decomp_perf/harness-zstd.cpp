#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>

#ifdef USE_LIBDEFLATE
#include "libdeflate/libdeflate.h"
#else
#include <zstd.h>
#endif

char input_buffer[64 * 1024 * 1024];
char output_buffer[64 * 1024 * 1024];

uint64_t nano() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
}


int main(int argc, char **argv) {

   FILE *f = fopen(argv[1], "rb");

   int iterations_per_run = atoi(argv[2]);
   int runs = atoi(argv[3]), ret, size;
   int compressed_size = fread(input_buffer, 1, sizeof input_buffer, f);

   size_t outlen = sizeof output_buffer;
   int status = ZSTD_decompress((uint8_t*)output_buffer, outlen, (uint8_t*)input_buffer, compressed_size);

   double times[10000], bands[10000];

   for (int j = 0; j < runs; j++) {
      {
         uint64_t start = nano();
         for (int i = 0; i < iterations_per_run; i++) {
            outlen = sizeof output_buffer;
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
   double mean = std::accumulate(&times[0], &times[runs], 0)/runs;
   std::sort(&bands[0], &bands[runs]);
//    printf("%s-decomp(ns),median:%lf,mean:%lf,max:%lf,min:%lf\n", argv[1],times[runs/2], mean, times[runs-2], times[0]);
   printf("%lf,%lf,%lf\n",(1.0 * size / compressed_size), (times[runs/2]), bands[runs/2]);

//    printf("%lf\n", compressed_size * iterations_per_run / (times[0] / 1000.0));
   
   return 0;
}