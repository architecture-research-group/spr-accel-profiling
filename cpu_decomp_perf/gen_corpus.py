from  zlib_compress import *
import os
import glob
import random


def gen_corpus_google_size_level(size, level):
    folder = 'HyperCompressBench/extracted_benchmarks'
    outdir = 'google-corpus'
    corpus = glob.glob(folder+'/*/*')
    kb4 = [file for file in corpus if os.stat(file).st_size == size]
    for i in range(len(kb4)):
        file = kb4[i]
        size = os.stat(file).st_size
        outfile = outdir + '/' + os.path.basename(file) + '_zstd' + str(level) + '_' + str(size)
        # if ( os.path.isfile(outfile) ):
        zstd_compress_file_level(file, outfile, level)

def gen_corpus_google_size_level_gzip(size, level):
    folder = 'HyperCompressBench/extracted_benchmarks'
    outdir = 'google-corpus'
    corpus = glob.glob(folder+'/*/*')
    kb4 = [file for file in corpus if os.stat(file).st_size == size]
    for i in range(len(kb4)):
        file = kb4[i]
        size = os.stat(file).st_size
        outfile = outdir + '/' + os.path.basename(file) + '_gz' + str(level) + '_' + str(size)
        # if ( os.path.isfile(outfile) ):
        gzip_compress_file_level(file, outfile, level)

def gen_corpus_google_size(size):
    folder = 'HyperCompressBench/extracted_benchmarks'
    outdir = 'google-corpus'
    corpus = glob.glob(folder+'/*/*')
    kb4 = [file for file in corpus if os.stat(file).st_size == size]
    for i in range(len(kb4)):
        file = kb4[i]
        size = os.stat(file).st_size
        outfile = outdir + '/' + os.path.basename(file) + '_zstd1_' + str(size)
        # if ( os.path.isfile(outfile) ):
        zstd_compress_file(file, outfile)

def gen_corpus_google():
    folder = 'HyperCompressBench/extracted_benchmarks'
    outdir = 'google-corpus'
    num_files = 10
    corpus = glob.glob(folder+'/*/*')
    for i in range(num_files):
        file = random.choice(corpus)
        size = os.stat(file).st_size
        outfile = outdir + '/' + os.path.basename(file) + '_' + str(size)
        # print(file, outfile)
        zstd_compress_file(file, outfile)

def gen_corpus_google_4kb():
    folder = 'HyperCompressBench/extracted_benchmarks'
    outdir = 'google-corpus'
    num_files = 10
    corpus = glob.glob(folder+'/*/*')
    kb4 = [file for file in corpus if os.stat(file).st_size == 4096]
    for i in range(len(kb4)):
        file = kb4[i]
        size = os.stat(file).st_size
        outfile = outdir + '/' + os.path.basename(file) + '_' + str(size)
        print(filje, outfile)
        zstd_compress_file(file, outfile)

def gen_corpus_google_16kb():
    folder = 'HyperCompressBench/extracted_benchmarks'
    outdir = 'google-corpus'
    num_files = 10
    corpus = glob.glob(folder+'/*/*')
    kb4 = [file for file in corpus if os.stat(file).st_size == (16*1024)]
    for i in range(len(kb4)):
        file = kb4[i]
        size = os.stat(file).st_size
        outfile = outdir + '/' + os.path.basename(file) + '_' + str(size)
        print(file, outfile)
        zstd_compress_file(file, outfile)

def gen_corpus_google_4kb_lz4():
    folder = 'HyperCompressBench/extracted_benchmarks'
    outdir = 'google-corpus'
    num_files = 10
    corpus = glob.glob(folder+'/*/*')
    kb4 = [file for file in corpus if os.stat(file).st_size == (4*1024)]
    for i in range(len(kb4)):
        file = kb4[i]
        size = os.stat(file).st_size
        outfile = outdir + '/' + os.path.basename(file) + '_lz4_' + str(size)
        print(file, outfile)
        zstd_compress_file(file, outfile)

