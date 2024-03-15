LZGEN=./lzdatagen/lzdgen 
[ ! -f $LZGEN ] && cd lzdatagen && make -j && cd ..
$LZGEN  -s 4m -r 4.0 foo.bin
