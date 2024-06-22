# MemAlloc

Re-writing the OS Coursework project - Linux Memory manager

for running the memory manager,

gcc -g -c testapp.c -o testapp.o
gcc -g -c mm.c -o mm.o
gcc -g -c gluethread/glthread.c -o gluethread/glthread.o
gcc -g gluethread/glthread.o mm.o testapp.o -o test.exe
