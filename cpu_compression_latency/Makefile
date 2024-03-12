all: isal_zlib_compress isal_zlib_decompress zlib_decompress isal_zlib_throughput
.PHONY: isal_zlib_compress isal_zlib_decompress zstd_compress clean  isal_zlib_throughput

# isal latency
isal_zlib_decompress: harness-isal_zlib_decompress.cpp
	g++ -o  isal_zlib_decompress $(CFLAGS) -Iisa-l/include harness-isal_zlib_decompress.cpp -lisal -lz

# zlib latency
zlib_decompress: harness-zlib_decompress.cpp
	g++ -o  zlib_decompress -Iisa-l/include harness-zlib_decompress.cpp -lz


isal_zlib: harness-isal_zlib.cpp
	g++ -o  isal_zlib $(CFLAGS) -Iisa-l/include harness-isal_zlib.cpp -lisal -lz

# anomolous <1us case when using same buffer in calls
isal_zlib_decompress-single-buffer: harness-isal_zlib_decompress-single-buffer.cpp
	g++ -o  isal_zlib_decompress-single-buffer $(CFLAGS) -Iisa-l/include harness-isal_zlib_decompress-single-buffer.cpp -lisal -lz

zstd_compress: harness-zstd-compress.cpp
	g++ -o zstd_compress  harness-zstd-compress.cpp -lzstd

clean:
	rm -f zstd_compress isal_zlib_decompress zlib_decompress isal_zlib
