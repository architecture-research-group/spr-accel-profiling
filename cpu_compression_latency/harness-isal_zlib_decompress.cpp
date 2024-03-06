#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cassert>

#include "igzip_lib.h"

char input_buffer[64 * 1024 * 1024];
char output_buffer[64 * 1024 * 1024];
char output_buffer_2[64 * 1024 * 1024];
char level_buffer[64 * 1024 * 1024];

using namespace std;
uint64_t nanos() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
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


int main(int argc, char **argv) {

   FILE *f = fopen(argv[1], "rb");

   int iterations_per_run = 10;
   int size = atoi(argv[2]);
   int runs = atoi(argv[3]), ret;
   int orig_size = fread(input_buffer, 1, size, f);
   assert( orig_size == size );

   struct isal_zstream strm;
   isal_deflate_init(&strm);
   strm.next_in = (uint8_t *)input_buffer;
   strm.avail_in = size;
   strm.next_out = (uint8_t *)output_buffer;
   strm.avail_out = sizeof output_buffer;
   strm.end_of_stream = 1;
   isal_deflate(&strm);

   struct inflate_state state;
   isal_inflate_init(&state);
   state.next_in = (uint8_t *)output_buffer;
   state.avail_in = strm.total_out;
   state.next_out = (uint8_t *)output_buffer_2;
   state.avail_out = sizeof output_buffer_2;
   isal_inflate(&state);

   compare_buffers(output_buffer_2, input_buffer, size);
   
   int index;
   if((index = compare_buffers(output_buffer_2, input_buffer, size)) != -1) {
      printf("Error: decompressed buffer does not match original buffer at index %d\n", index);
      return 1;
   }
   
   uint64_t latency_avg, latency_min, latency_max;
   vector<uint64_t> latencies(runs);
   for(int i=0; i<runs; i++){
      uint64_t start = nanos();
      for(int j=0; j<iterations_per_run; j++){
         state.next_in = (uint8_t *)output_buffer;
         state.avail_in = strm.total_out;
         state.next_out = (uint8_t *)output_buffer_2;
         state.avail_out = sizeof output_buffer_2;
         isal_inflate(&state);
      }
      uint64_t end = nanos();
      latencies.push_back(end - start);      
   }
   printf("DecompressAverageLatency(ns),%d,%f\n",
   size, 
   std::accumulate(latencies.begin(), 
   
   latencies.end(), 0) / (runs*1.0));
   
   
   return 0;
}

