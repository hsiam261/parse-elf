main.out: main.c libinvoke.so 
	gcc main.c -o main.out -L. -linvoke -Wl,-rpath,./

libinvoke.so: invoke.c libfunc.so
	gcc invoke.c -fPIC -c
	gcc invoke.o -shared -o libinvoke.so -L. -lfunc -Wl,-rpath,./

libfunc.so: func.c 
	gcc func.c -fPIC -c
	gcc func.o -shared -o libfunc.so

clean:
	rm *.o
	rm *.out 
	rm *.so

run:
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/. ./main.out
