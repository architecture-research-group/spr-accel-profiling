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
	},
]

TESTS = glob.glob('corpus/*.zlib6')
TESTS.sort(key=os.path.getsize)

for target in TARGETS:
	subprocess.run([CC] + target.get('cflags', []) + ['harness.cpp', target['library'], '-o', 'harness-' + target['name']], check=True)


for test in TESTS:

	last = None

	runs = 100

	results = []
	for target in TARGETS:

		r = subprocess.check_output(['./harness-' + target['name'], test, '1', str(runs)])
		print(r)
		# results.append('%.1f' % (r,))
		# if last is not None:
		# 	results[-1] += (' (%.3fx)' % (r / last,))
		# last = r

	print('\t'.join([test.split('/')[-1]] + results))