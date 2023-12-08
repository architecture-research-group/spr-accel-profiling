import os

def zlib_compress_file(infile):
    import zlib
    level=6
    outfile = infile + '.zlib' + str(level)
    with open('rand.txt', 'rb') as f:
        data = f.read()
        comp = zlib.compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)

def zstd_compress_file(infile):
    import zstd
    level=1
    outfile = './corpus/' + os.path.basename(infile) + '.zst' + str(level)
    with open(infile, 'rb') as f:
        data = f.read()
        comp = zstd.ZSTD_compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)

def lz4_compress_file(infile):
    import lz4.frame
    # level=1
    outfile = './corpus/' + os.path.basename(infile) + '.lz4' #+ str(level)
    with open(infile, 'rb') as f:
        data = f.read()
        comp = lz4.frame.compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)