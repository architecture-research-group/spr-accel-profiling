import zlib
import zstd
import os

def zlib_compress_file(infile):
    level=6
    outfile = infile + '.zlib' + str(level)
    with open('rand.txt', 'rb') as f:
        data = f.read()
        comp = zlib.compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)

def zstd_compress_file(infile):
    level=1
    outfile = './corpus/' + os.path.basename(infile) + '.zst' + str(level)
    with open(infile, 'rb') as f:
        data = f.read()
        comp = zstd.ZSTD_compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)