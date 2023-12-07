import zlib

def compress_file(infile):
    level=6
    outfile = infile + '.zlib' + str(level)
    with open('rand.txt', 'rb') as f:
        data = f.read()
        comp = zlib.compress(data)
        with open(outfile,'wb') as of:
            of.write(comp)