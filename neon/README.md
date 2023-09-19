# neon
g++ -DTEST_MAIN -march=armv8-a+crypto -O2 -static sha256-armteststringfile.c -o sha256-armteststringfile.exe
armv8-a+crypto代表armv8-a的架构并且使用crypto密码拓展 -static静态编译 sha256-armteststringfile.exe是neon优化的可执行文件程序，需要支持simd的neon以及sha指令集才能使用
gcc -DTEST_MAIN -std=c99 -O2 -static sha256testfilestring.c -o sha256testfilestring.exe
sha256testfilestring.exe是arm架构的未使用neon加速的程序

