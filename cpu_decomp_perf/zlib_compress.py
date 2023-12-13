import os
import gzip    


def zlib_compress_file(infile):
    import zlib
    level=6
    outfile = infile + '.zlib' + str(level)
    with open('rand.txt', 'rb') as f:
        data = f.read()
        comp = zlib.compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)


def zstd_compress_file(infile, outfile):
    import zstd
    level=1
    with open(infile, 'rb') as f:
        data = f.read()
        comp = zstd.ZSTD_compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)

def zstd_compress_file_level(infile, outfile, level):
    import zstd
    with open(infile, 'rb') as f:
        data = f.read()
        comp = zstd.ZSTD_compress(data, level)
        with open(outfile,'wb') as of:
            of.write(comp)

def gzip_compress_file_level(infile, outfile, level):
    with open(infile, 'rb') as f:
        data = f.read()
        with gzip.open(outfile,'wb', level) as of:
            of.write(data)

def lz4_compress_file(infile, outfile):
    import zstd
    level=1
    with open(infile, 'rb') as f:
        data = f.read()
        comp = lz4.frame.compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)