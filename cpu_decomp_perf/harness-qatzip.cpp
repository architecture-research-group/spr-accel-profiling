#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>

#ifdef USE_LIBDEFLATE
#include "libdeflate/libdeflate.h"
#else
#include <qatzip.h>
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
   unsigned int compressed_size = fread(input_buffer, 1, sizeof input_buffer, f);

   int rc;
   QzSession_T sess;
   rc = qzInit(&sess, 0);
   if (QZ_OK != rc) {
      return rc;
   }
   QzSessionParams_T params;
   params.direction = QZ_DIR_DECOMPRESS;
   params.huffman_hdr = QZ_STATIC_HDR;
   params.data_fmt = QZ_DEFLATE_GZIP;
   params.comp_algorithm = QZ_DEFLATE;
   params.comp_lvl = 6;
   params.max_forks = 0;
   params.hw_buff_sz = 8192; // must be power of 2
   params.strm_buff_sz = params.hw_buff_sz;
   params.input_sz_thrshold = QZ_COMP_THRESHOLD_MINIMUM; // no sw fallback
   params.req_cnt_thrshold = 32; // qatzip_internal.h: NUM_BUFF= 32
   params.wait_cnt_thrshold = 0; //retry immediately


   qzSetupSession(&sess, &params);
   unsigned int outlen = sizeof output_buffer;
   int status = qzDecompress(&sess, (uint8_t*)input_buffer, &compressed_size, (uint8_t*)output_buffer, &outlen);
   if (status != QZ_OK) {
         printf("ERROR: Decompression FAILED with return value: %d\n", rc);
   }
   double times[10000], bands[10000];

   for (int j = 0; j < runs; j++) {
      {
         uint64_t start = nano();
         for (int i = 0; i < iterations_per_run; i++) {
            outlen = sizeof output_buffer;
            status = qzDecompress(&sess, (uint8_t*)input_buffer, &compressed_size, (uint8_t*)output_buffer, &outlen);
         }
         uint64_t end = nano();
         double bb = (end-start);
         times[j] = bb/iterations_per_run;
         // fprintf(stderr,"%d\n", size);
         double band =  (double)(outlen*iterations_per_run) / (end-start);
         bands[j] = band;
      }
   }

   qzTeardownSession(&sess);
   qzClose(&sess);

   std::sort(&times[0], &times[runs]);
   double mean = std::accumulate(&times[0], &times[runs], 0)/runs;
   std::sort(&bands[0], &bands[runs]);
//    printf("%s-decomp(ns),median:%lf,mean:%lf,max:%lf,min:%lf\n", argv[1],times[runs/2], mean, times[runs-2], times[0]);
   printf("%lf,%lf,%lf,%d,%d\n",(1.0 * outlen / compressed_size), (times[runs/2]), bands[runs/2],compressed_size, outlen);

//    printf("%lf\n", compressed_size * iterations_per_run / (times[0] / 1000.0));
   
   return 0;
}