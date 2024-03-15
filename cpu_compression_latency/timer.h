#pragma once

#define MAX_EXPAND_ISAL(size) ISAL_DEF_MAX_HDR_SIZE + size
#define CALGARY "./calgary"
#include <fstream>
#include <vector>
#include <chrono>
#include <cassert>

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



int corpus_to_input_buffer(char ** &testBufs,int sizePerBuf, const char *filename) {
   FILE *file = fopen(filename, "r");
   if (file == NULL){
    printf("Failed to open file:%s\n", filename);
    exit(-1);
   }
   fseek(file, 0, SEEK_END);
   int size = ftell(file);
   fseek(file, 0, SEEK_SET);
   if (size < sizePerBuf){
       printf("Warning: corpus file is smaller than payload size -- results may be skewed\n");
   }
   int num_bufs = size / sizePerBuf ;

   testBufs = (char **)malloc(sizeof(char *) * num_bufs);

   for ( int i=0; i < num_bufs; i++ ){
       uint64_t offset=0;
       testBufs[i] = (char *)malloc(sizePerBuf);
       char *testBuf = testBufs[i];
fill_file:
       uint64_t readLen = fread((void *) (testBuf + offset), 1, sizePerBuf - offset, file);
       if ( readLen < sizePerBuf ){
           rewind(file);
           offset += readLen;
           goto fill_file;
       }
   }
   return num_bufs;
}
/*
 * This function reads the calgary corpus into a buffer and then splits it into
   * num_buffers sub-buffers. It returns the number of sub-buffers created.
*/
int calgary_corpus_to_input_buffer(char ** &testBufs,int sizePerBuf ) {
   FILE *file = fopen(CALGARY, "rb");
   fseek(file, 0, SEEK_END);
   int size = ftell(file);
   fseek(file, 0, SEEK_SET);
   int num_bufs = size / sizePerBuf;

   testBufs = (char **)malloc(sizeof(char *) * num_bufs);
   std::ifstream infile(CALGARY, std::ios::binary);
   for(int i=0; i< num_bufs; i++){
      testBufs[i] = (char *)malloc(sizePerBuf);
      if (!infile.read(testBufs[i], sizePerBuf)){
         printf("Error: Failed to read file\n");
         return -1;
      }
   }
   return num_bufs;
}


int compare_buffers(const char *a, const char *b, int size) {
   for (int i = 0; i< size; i++) {
      if (a[i] != b[i]) {
         return i;
      }
   }
   return -1;
}

void flush_buf(char *buf, int size) {
   for (int i = 0; i< size; i+=64) {
      _mm_clflush(&buf[i]);
   }
}

vector<uint64_t> WithoutHiLo(vector<uint64_t> orig)
{
     std::sort(orig.begin(), orig.end());
     vector<uint64_t> newVec = vector<uint64_t>(orig.size());
     std:copy(&orig[1], &orig[orig.size()-1], &newVec[0]);
     return newVec;
}
