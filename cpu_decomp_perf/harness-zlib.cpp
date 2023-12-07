#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>

#ifdef USE_LIBDEFLATE
#include "libdeflate/libdeflate.h"
#else
#include <zlib.h>
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
   int runs = atoi(argv[3]);
   int compressed_size = fread(input_buffer, 1, sizeof input_buffer, f);

   size_t outlen = sizeof output_buffer;
   int status = uncompress((uint8_t*)output_buffer, &outlen, (uint8_t*)input_buffer, sizeof input_buffer);

   double times[10000];

   for (int j = 0; j < runs; j++) {
      {
         uint64_t start = nano();
         for (int i = 0; i < iterations_per_run; i++) {
            outlen = sizeof output_buffer;
            uncompress((uint8_t*)output_buffer, &outlen, (uint8_t*)input_buffer, compressed_size);
         }
         uint64_t end = nano();
         double bb = (end-start);
         times[j] = bb;
        //  printf("%lfns\n", times[j]);
         double b = (end-start) / (double)(compressed_size*iterations_per_run) * 3.2;
      }
   }

   std::sort(&times[0], &times[runs]);
   double mean = std::accumulate(&times[0], &times[runs], 0)/runs;
//    printf("%s-decomp(ns),median:%lf,mean:%lf,max:%lf,min:%lf\n", argv[1],times[runs/2], mean, times[runs-2], times[0]);
   printf("%lf,%lf,%lf,%lf\n",times[runs/2], mean, times[runs-2], times[0]);

//    printf("%lf\n", compressed_size * iterations_per_run / (times[0] / 1000.0));
   
   return 0;
}