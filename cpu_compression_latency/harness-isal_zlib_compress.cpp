#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cassert>

#include "igzip_lib.h"

char input_buffer[64 * 1024 * 1024];
char output_buffer[64 * 1024 * 1024];
char level_buffer[64 * 1024 * 1024];

uint64_t nano() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
}


int main(int argc, char **argv) {

   FILE *f = fopen(argv[1], "rb");

   int iterations_per_run = 10;
   int size = atoi(argv[2]);
   int runs = atoi(argv[3]), ret;
   int orig_size = fread(input_buffer, 1, size, f);
   assert( orig_size == size );

   /*
struct isal_zstream {
	uint8_t *next_in;	//!< Next input byte
	uint32_t avail_in;	//!< number of bytes available at next_in
	uint32_t total_in;	//!< total number of bytes read so far

	uint8_t *next_out;	//!< Next output byte
	uint32_t avail_out;	//!< number of bytes available at next_out
	uint32_t total_out;	//!< total number of bytes written so far

	struct isal_hufftables *hufftables; //!< Huffman encoding used when compressing
	uint32_t level; //!< Compression level to use
	uint32_t level_buf_size; //!< Size of level_buf
	uint8_t * level_buf; //!< User allocated buffer required for different compression levels
	uint16_t end_of_stream;	//!< non-zero if this is the last input buffer
	uint16_t flush;	//!< Flush type can be NO_FLUSH, SYNC_FLUSH or FULL_FLUSH
	uint16_t gzip_flag; //!< Indicate if gzip compression is to be performed
	uint16_t hist_bits; //!< Log base 2 of maximum lookback distance, 0 is use default
	struct isal_zstate internal_state;	//!< Internal state for this stream
	
};
*/



for (int level=ISAL_DEF_MIN_LEVEL; level !=ISAL_DEF_MAX_LEVEL; level+=1){
   double times[runs], bands[runs];

   struct isal_zstream strm;
   isal_deflate_init(&strm);
   strm.next_in = (uint8_t *)input_buffer;
   strm.avail_in = size;
   strm.next_out = (uint8_t *)output_buffer;
   strm.avail_out = sizeof output_buffer;
   strm.level_buf_size=sizeof level_buffer;
   strm.level_buf=(uint8_t *)level_buffer;
   strm.level = 1;

   int status = isal_deflate(&strm);
   if( status != COMP_OK ){
		printf("Error: %d\n",status);
	   exit(-1);
	}
   for (int j = 0; j < runs; j++) {
         uint64_t start = nano();
         for (int i = 0; i < iterations_per_run; i++) {
		   strm.next_in = (uint8_t *)input_buffer;
		   strm.avail_in = size;
		   strm.next_out = (uint8_t *)output_buffer;
		   strm.avail_out = sizeof output_buffer;
		   strm.end_of_stream = 1;
		   strm.level = level;
		   strm.total_out = 0;
            int status = isal_deflate_stateless(&strm);
		   if( status != COMP_OK ){
				printf("Error: %d\n",status);
			   exit(-1);
			}
         }
         uint64_t end = nano();
         double bb = (end-start);
         times[j] = bb/iterations_per_run;
         double band =  (double)(size*iterations_per_run) / (end-start);
         bands[j] = band;
   }

   std::sort(&times[0], &times[runs-1]);
   double sum = std::accumulate(&times[0], &times[runs-1], 0);
   std::sort(&bands[0], &bands[runs-1]);
   printf("%d,%lf,%lf,%lf,%lf,%d,%d\n",level,(1.0 * orig_size / strm.total_out), (times[runs/2]), sum/runs, bands[runs/2],orig_size, strm.total_out);
   }
   
   return 0;
}

