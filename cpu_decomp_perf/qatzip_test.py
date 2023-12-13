import zlib_compress
from gen_corpus import *
import subprocess
import os
import glob

CC = 'g++'
TARGETS = [
	#{
	#	'name': 'libdeflate-old',
	#	'library': 'libdeflate-old/libdeflate.a',
	#	'cflags': ['-DUSE_LIBDEFLATE'],
	#},
	# {
	# 	'name': 'zlib',
	# 	'library': '-lz',
	# 	'cflags': [ ],
	# 	'tests': glob.glob('corpus/*.zlib6')
	# },
	# {
	# 	'name': 'zstd',
	# 	'library': '-lzstd',
	# 	'size': 4*1024,
	# 	'level': 22,
	# 	'cflags': [ ],
	# 	'tests': glob.glob('google-corpus/*_zstd' + str(level) + '_' + str(size))
	# },
	{
		'name': 'qatzip',
		'library': '-lqatzip',
		'cflags': [ '-fpermissive' ],
		'size': 4*1024,
		'level': 6,
		'corpus_gen': lambda x, y: print("No Filegen Necessary"),
		'tests': glob.glob('HyperCompressBench/extracted_benchmarks/*/*'),
	},
	# {
	# 	'name': 'lz4',
	# 	'library': '-llz4',
	# 	'cflags': [ ],
	# 	'tests': glob.glob('google-corpus/*_lz4_*')
	# },
	# {
	# 	'name': 'isal_zlib',
	# 	'library': '-lisal',
	# 	'cflags': [ "-Iisa-l/include" ],
	# 	'tests': glob.glob('corpus/*.zlib6')
	# },

]


for target in TARGETS:
	size = target['size']
	level = target['level']
	target['corpus_gen'](target['size'],target['level'])
	subprocess.run([CC] + target.get('cflags', []) + ['harness-' + target['name'] + '.cpp', target['library'], '-o', 'harness-' + target['name']], check=True)
	TESTS = target['tests']
	TESTS.sort(key=os.path.getsize)

	with open('result.csv', 'w') as f:

		for test in TESTS:

			last = None

			runs = 100

			results = []
			for target in TARGETS:

				# r = subprocess.check_output( [ target['root'], './harness-' + target['name'], test, '10', str(runs)]).decode("utf-8")[:-1]
				r = subprocess.check_output( [ "sudo", "QZ_ROOT=QATzip;", "-E", "taskset", "-c", "0", "./QATzip/test/test", "-m", "4", "-l", str(runs), "-t", "1", "-D", "decomp", "-P", "busy", "-i",  test]).decode("utf-8")[:-1]
				
				f.write(target['name']+','+test+','+ r +'\n')
