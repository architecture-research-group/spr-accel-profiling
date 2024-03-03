all: isal_zlib_compress
.PHONY: isal_zlib_compress zstd_compress clean 

isal_zlib_compress: harness-isal_zlib_compress.cpp
	g++ -o  isal_zlib_compress -Iisa-l/include harness-isal_zlib_compress.cpp -lisal

zstd_compress: harness-zstd-compress.cpp
	g++ -o zstd_compress  harness-zstd-compress.cpp -lzstd

clean:
	rm -f zstd_compress
