.PHONY: zstd_compress clean
zstd_compress: harness-zstd-compress.cpp
	g++ -o zstd_compress harness-zstd-compress.cpp -lzstd
clean:
	rm -f zstd_compress
