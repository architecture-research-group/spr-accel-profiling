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

// char input_buffer[64 * 1024 * 1024];
// char output_buffer[64 * 1024 * 1024];
// char output_buffer_2[64 * 1024 * 1024];
char level_buffer[64 * 1024 * 1024];

using namespace std;
uint64_t nanos() {
   return std::chrono::duration_cast< ::std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
}

uint64_t micros() {
   return std::chrono::duration_cast< ::std::chrono::microseconds>(
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

void flush_buf(char *buf, int size) {
   for (int i = 0; i < size; i+=64) {
      _mm_clflush(&buf[i]);
   }
}

/*
 * populate list of buffers with sub-buffers of size size/num_buffers using contents of input_buffer
*/
void generate_list_of_buffers(char *input_buffer, int size, int num_buffers, char **buffers) {
   for (int i = 0; i < num_buffers; i++) {
      buffers[i] = (char *)malloc(size / num_buffers);
      memcpy(buffers[i], input_buffer + (i * (size / num_buffers)), size/num_buffers);
   }
}

/*
 * This function reads the calgary corpus into a buffer and then splits it into
   * num_buffers sub-buffers. It returns the number of sub-buffers created.
*/
int corpus_to_input_buffer(char ** &testBufs,int sizePerBuf ) {
   FILE *file = fopen(CALGARY, "rb");
   fseek(file, 0, SEEK_END);
   int size = ftell(file);
   fseek(file, 0, SEEK_SET);
   int max_bufs = size / sizePerBuf;
   testBufs = (char **)malloc(sizeof(char *) * max_bufs);
   std::ifstream infile(CALGARY, std::ios::binary);
   int i=0;
   if (file) {
      while (true) {
         testBufs[i] = (char *)malloc(sizePerBuf);
         if (!infile.read(testBufs[i], sizePerBuf)) { /*fail if not enough content*/
               break;
         }
         i++;
      }
   }
   return i;
}


int main(int argc, char **argv) {


   int size = atoi(argv[1]);
   int do_flush = atoi(argv[2]), ret;
   int iterations_per_run = atoi(argv[3]);
   int discard_fst = atoi(argv[4]);


   char **input_buffers;
   int num_bufs = corpus_to_input_buffer(input_buffers, size);
   char **output_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   uint32_t compressed_sizes[num_bufs];
   char **output_buffers_2 = (char **)malloc(sizeof(char *) * num_bufs);
   char **backup_buffers = (char **)malloc(sizeof(char *) * num_bufs);
   for(int i=0; i<num_bufs; i++){
      output_buffers[i] = (char *)malloc(size);
      output_buffers_2[i] = (char *)malloc(size);
   }

   printf("Testing on %d buffers\n", num_bufs);

   /* DEFLATE */
   struct isal_zstream stream;
   isal_deflate_init(&stream);

   for(int i=0; i< num_bufs; i++){
      backup_buffers[i] = (char *)malloc(size);
      stream.next_in = (uint8_t *)input_buffers[i];
      stream.avail_in = size;
      stream.next_out = (uint8_t *)output_buffers[i];
      stream.avail_out = size;
      stream.end_of_stream = 0;
      stream.flush = SYNC_FLUSH;
      isal_deflate(&stream);
      compressed_sizes[i] = stream.total_out;
      backup_buffers[i] = (char *)malloc(stream.total_out);
      memcpy(backup_buffers[i], output_buffers[i], stream.total_out);
      printf("Compressed buffer %d   to %d\n", i, stream.total_out);
      stream.total_out = 0;
   }

   iterations_per_run=1;
   uint64_t latency_avg, latency_min, max_latency;
   vector<uint64_t> latencies(num_bufs);

   /* INFLATE */
   struct inflate_state state;
   for(int i=0; i<num_bufs; i++){      
      if(do_flush){
         flush_buf(output_buffers[i], compressed_sizes[i]);
         flush_buf(output_buffers_2[i], size);
      }
      uint64_t start = micros();
      for(int j=0; j<iterations_per_run; j++){
         state.next_in = (uint8_t*)output_buffers[i];
         state.avail_in = compressed_sizes[i];
         state.next_out = (uint8_t*)output_buffers_2[i];
         state.avail_out = size;
         isal_inflate(&state);
      }
      uint64_t end = micros();
      // printf("latency,%ld\n", end-start);
      int mismatch;
      if( (mismatch = compare_buffers(output_buffers_2[i], input_buffers[i], size)) != -1){
         printf("Error: Decompression mismatch buffer:%d index:%d \n", i, mismatch );
         // return -1;
      }
      latencies[i]=((end - start)/iterations_per_run);      
   }
   max_latency  = *max_element(&(latencies[0]), &(latencies[num_bufs-1]));
   printf("ISAL-Decompress,%d,,%f,%ld,\n",
   size, 
   std::accumulate(latencies.begin()+discard_fst,
      std::next(latencies.begin(),num_bufs-discard_fst), 0) / (num_bufs*1.0),
      max_latency); 
   int stateful_deviations=0;
   for(int i=0; i<num_bufs; i++){
      if(do_flush){
         flush_buf(output_buffers[i], size);
         flush_buf(input_buffers[i], size);
      }
      
      uint64_t start = micros();
      stream.total_out = 0;
      for(int j=0; j<iterations_per_run; j++){
         stream.next_in = (uint8_t *)input_buffers[i];
         stream.avail_in = size;
         stream.next_out = (uint8_t *)output_buffers[i];
         stream.avail_out = size;
         stream.end_of_stream = 0;
         stream.flush = SYNC_FLUSH;
         isal_deflate(&stream);
      }
      uint64_t end = micros();
      // printf("latency,%ld\n", end-start);
      if(compare_buffers(output_buffers[i], backup_buffers[i], stream.total_out) != -1){
         stateful_deviations+=1;
      }
      latencies[i] = (end - start)/iterations_per_run;      
   }
   max_latency = *max_element(&(latencies[0]), &(latencies[num_bufs-1]));
   double avg_ratio = std::accumulate(&compressed_sizes[0],&compressed_sizes[num_bufs-1],0)/(num_bufs * size * 1.0);
   printf("ISAL-Compress,%d,%f,%f,%ld,%d\n",
      size,
      // size*1.0/stream.total_out,
      avg_ratio,
      std::accumulate(latencies.begin()+discard_fst, 
         std::next(latencies.begin(),
            num_bufs-discard_fst), 0)
      / (num_bufs*1.0),
      max_latency,
      stateful_deviations);  

   

   
   

   // latencies.clear();
    

   // /* Zlib */
   // z_stream strm;
   // strm.zalloc = Z_NULL;
   // strm.zfree = Z_NULL;
   // strm.opaque = Z_NULL;
   // ret = deflateInit(&strm, 3);
   // if (ret != Z_OK)
   //      return ret;

   // latencies.clear();
   // for(int i=0; i<num_bufs; i++){
   //    if(do_flush){
   //       flush_buf(output_buffer, sizeof output_buffer);
   //       flush_buf(input_buffer, sizeof input_buffer);
   //       _mm_mfence();
   //    }
   //    uint64_t start = micros();
   //    for(int j=0; j<iterations_per_run; j++){
   //       strm.avail_in = size;
   //       strm.next_in = (Bytef *)input_buffer;
   //       strm.next_out = (Bytef *)output_buffer;
   //       strm.avail_out = sizeof output_buffer;
   //       deflate(&strm, Z_FINISH);
   //    }
   //    uint64_t end = micros();
   //    // assert(compare_buffers(output_buffer, backup_buffer, size) == -1);
   //    latencies[i] = ((end - start)/iterations_per_run);    
   //    printf("latency,%ld\n", end-start);  
   // }

   // max_latency = *max_element(&(latencies[0]), &(latencies[num_bufs-1]));
   // printf("Zlib-Compress,%d,%f,%f,%ld\n",
   // size,
   // size*1.0/strm.total_out,
   // std::accumulate(latencies.begin()+discard_fst,
   //    std::next(latencies.begin(),num_bufs-discard_fst), 0) / (num_bufs*1.0),
   // max_latency);

   
   return 0;
}

