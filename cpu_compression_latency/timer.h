#pragma once

#define MAX_EXPAND_ISAL(size) ISAL_DEF_MAX_HDR_SIZE + size
#define CALGARY "./calgary"
#include <fstream>


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


int corpus_to_input_buffer(char ** &testBufs,int sizePerBuf ) {
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

int compare_buffers(const char *a, const char *b, int size) {
   for (int i = 0; i < size; i++) {
      if (a[i] != b[i]) {
         return i;
      }
   }
   return -1;
}