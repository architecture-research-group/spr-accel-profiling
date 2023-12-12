import zlib_compress
from gen_corpus import *
import subprocess
import os
import glob

size = 32 * 1024
level = 22
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
	{
		'name': 'zstd',
		'library': '-lzstd',
		'cflags': [ ],
		'tests': glob.glob('google-corpus/*_zstd' + str(level) + '_' + str(size))
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

# gen_corpus_google_size_level(size, level)

for target in TARGETS:
	subprocess.run([CC] + target.get('cflags', []) + ['harness-' + target['name'] + '.cpp', target['library'], '-o', 'harness-' + target['name']], check=True)
	TESTS = target['tests']
	TESTS.sort(key=os.path.getsize)

	with open('result.csv', 'w') as f:

		for test in TESTS:

			last = None

			runs = 100

			results = []
			for target in TARGETS:

				r = subprocess.check_output(['./harness-' + target['name'], test, '10', str(runs)]).decode("utf-8")[:-1]
				f.write(target['name']+','+test+','+ r +'\n')
			# results.append('%.1f' % (r,))
			# if last is not None:
			# 	results[-1] += (' (%.3fx)' % (r / last,))
			# last = r

	# print('\t'.join([test.split('/')[-1]] + results))