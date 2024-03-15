LZGEN=./lzdatagen/lzdgen 
[ ! -f $LZGEN ] && cd lzdatagen && make -j && cd ..
$LZGEN  -s 3m -r 4.0 foo.bin
