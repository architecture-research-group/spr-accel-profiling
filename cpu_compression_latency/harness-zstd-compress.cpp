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

   int iterations_per_run = 1;
   int size = atoi(argv[2]);
   int runs = atoi(argv[3]), ret;
   int orig_size = fread(input_buffer, 1, size, f);

   size_t outlen = sizeof output_buffer;
   int status = ZSTD_compress((uint8_t*)output_buffer, outlen, (uint8_t*)input_buffer, orig_size, 6);

   double times[10000], bands[10000];

   for (int j = 0; j < runs; j++) {
      {
         uint64_t start = nano();
         for (int i = 0; i < iterations_per_run; i++) {
            outlen = sizeof output_buffer;
            size = ZSTD_compress((uint8_t*)output_buffer, outlen, (uint8_t*)input_buffer, orig_size, 6);
         }
         uint64_t end = nano();
         double bb = (end-start);
         times[j] = bb/iterations_per_run;
         double band =  (double)(size*iterations_per_run) / (end-start);
         bands[j] = band;
      }
   }

   std::sort(&times[0], &times[runs]);
   double sum = std::accumulate(&times[0], &times[runs], 0);
   std::sort(&bands[0], &bands[runs]);
   printf("Ratio,median(ns),avg(ns),BW(GB/s),payloadsize(B),compressedSize(B)\n");
   printf("%lf,%lf,%lf,%lf,%d,%d\n",(1.0 * size / orig_size), (times[runs/2]), sum/runs, bands[runs/2],orig_size, size);
   return 0;
}
