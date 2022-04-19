clear
rm formatfs.exe
gcc formatfs.c filesystem.c softwaredisk.c -o formatfs.exe
./formatfs.exe
