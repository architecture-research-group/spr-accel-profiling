import zlib_compress
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
	{
		'name': 'zlib',
		'library': '-lz',
		'cflags': [ ],
		'tests': glob.glob('corpus/*.zlib6')
	},
	{
		'name': 'zstd',
		'library': '-lzstd',
		'cflags': [ ],
		'tests': glob.glob('corpus/*.zst1')
	},
]



for target in TARGETS:
	subprocess.run([CC] + target.get('cflags', []) + ['harness-' + target['name'] + '.cpp', target['library'], '-o', 'harness-' + target['name']], check=True)
	TESTS = target['tests']
	TESTS.sort(key=os.path.getsize)

	for test in TESTS:

		last = None

		runs = 100

		results = []
		for target in TARGETS:

			r = str(subprocess.check_output(['./harness-' + target['name'], test, '1', str(runs)]))
			print(test+','+r)
			# results.append('%.1f' % (r,))
			# if last is not None:
			# 	results[-1] += (' (%.3fx)' % (r / last,))
			# last = r

	print('\t'.join([test.split('/')[-1]] + results))