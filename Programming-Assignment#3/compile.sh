#TEST CASES ATTACHED.  Important:  testfs0, testfs1, 2, 3 are all standalone.  
#Run formatfs before each.   testfs4a/b are designed to be executed in sequence 
#without an intervening formatfs, so for that pair, 
#you should:  ./formatfs && ./testfs4a && ./testfs4b

#clearing console
clear

#remove all previous executables
rm formatfs.exe

#compile and run initial disk formater
gcc -Wall -g formatfs.c filesystem.c softwaredisk.c -o formatfs.exe
./formatfs.exe

echo "------------------------------------------------------------"

#compile testfs0 test case
gcc -Wall -g testfs0.c filesystem.c softwaredisk.c -o testfs0.exe
./testfs0.exe
./formatfs.exe

echo "------------------------------------------------------------"

#compile testfs1 test case
gcc -Wall -g testfs1.c filesystem.c softwaredisk.c -o testfs1.exe
./testfs1.exe
./formatfs.exe

echo "------------------------------------------------------------"

#compile testfs2 test case
gcc -Wall -g testfs2.c filesystem.c softwaredisk.c -o testfs2.exe
./testfs2.exe
./formatfs.exe

echo "------------------------------------------------------------"

#compile testfs2 test case
gcc -Wall -g testfs3.c filesystem.c softwaredisk.c -o testfs3.exe
./testfs3.exe