make test
./build/e ./test/ysj_if.m
cp ./test/ysj_if.s object
cd object
./asm ysj_if.s
./machine ysj_if.o