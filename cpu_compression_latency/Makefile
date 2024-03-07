all: isal_zlib_compress isal_zlib_decompress zlib_decompress isal_zlib_throughput
.PHONY: isal_zlib_compress isal_zlib_decompress zstd_compress clean  isal_zlib_throughput

isal_zlib_throughput: harness-isal_zlib_throughput.cpp
	g++ -o  isal_zlib_throughput $(CFLAGS) -Iisa-l/include harness-isal_zlib_throughput.cpp -lisal -lz

isal_zlib_compress: harness-isal_zlib_compress.cpp
	g++ -o  isal_zlib_compress $(CFLAGS) -Iisa-l/include harness-isal_zlib_compress.cpp -lisal

isal_zlib_decompress: harness-isal_zlib_decompress.cpp
	g++ -o  isal_zlib_decompress $(CFLAGS) -Iisa-l/include harness-isal_zlib_decompress.cpp -lisal -lz

isal_zlib_decompress-single-buffer: harness-isal_zlib_decompress-single-buffer.cpp
	g++ -o  isal_zlib_decompress-single-buffer $(CFLAGS) -Iisa-l/include harness-isal_zlib_decompress-single-buffer.cpp -lisal -lz

zlib_decompress: harness-zlib_decompress.cpp
	g++ -o  zlib_decompress -Iisa-l/include harness-zlib_decompress.cpp -lz

zstd_compress: harness-zstd-compress.cpp
	g++ -o zstd_compress  harness-zstd-compress.cpp -lzstd

clean:
	rm -f zstd_compress
