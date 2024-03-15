LZGEN=./lzdatagen/lzdgen 
[ ! -f $LZGEN ] && cd lzdatagen && make -j && cd ..
$LZGEN -r 1.0 no_lz.bin
