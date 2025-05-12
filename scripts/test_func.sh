make test
./build/e ./test/ysj_func.m
cp ./test/ysj_func.s object
cd object
./asm ysj_func.s
./machine ysj_func.o