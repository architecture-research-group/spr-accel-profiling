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

char input_buffer[64 * 1024 * 1024];
char output_buffer[64 * 1024 * 1024];

uint64_t nano() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
}

int uncompress(void *out, size_t *size, void *in, size_t insize) {
   struct inflate_state state;
	isal_inflate_init(&state);
	state.next_out = (uint8_t *)out;
	state.avail_out = *size;
	state.next_in = (uint8_t *)in;
	state.avail_in = insize;
	state.crc_flag = ISAL_ZLIB;
	int status = isal_inflate_stateless(&state);
	if (status != ISAL_DECOMP_OK) {
		printf("isal_inflate_stateless failed %d\n", status);
		return 1;
	}
	*size = state.total_out;
	return 0;
}

int main(int argc, char **argv) {

   FILE *f = fopen(argv[1], "rb");

   int iterations_per_run = atoi(argv[2]);
   int runs = atoi(argv[3]);
   int compressed_size = fread(input_buffer, 1, sizeof input_buffer, f);

   size_t outlen = sizeof output_buffer;
   int status = uncompress((uint8_t*)output_buffer, &outlen, (uint8_t*)input_buffer, sizeof input_buffer);

   double times[100];

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
         double b = (end-start) / (double)(compressed_size*iterations_per_run) * 3.2;
      }
   }

   std::sort(&times[0], &times[runs]);
   
   printf("%lf\n", compressed_size * iterations_per_run / (times[0] / 1000.0));
   
   return 0;
}