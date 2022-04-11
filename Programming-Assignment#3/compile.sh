clear
rm formatfs.exe
gcc -Wall -g formatfs.c filesystem.c softwaredisk.c -o formatfs.exe
