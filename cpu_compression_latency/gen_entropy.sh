LZGEN=./lzdatagen/lzdgen 
[ ! -f $LZGEN ] && cd lzdatagen && make -j && cd ..
$LZGEN -s 3m -r 1.0 no_lz.bin
