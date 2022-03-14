main.out: main.c libinvoke-foo-bar.so  
	gcc main.c -o main.out -L. -linvoke-foo-bar

build-with-runpath: main.c libinvoke-foo-bar.so 
	gcc main.c -o mainr.out -L. -linvoke-foo-bar -Wl,-rpath,./

libinvoke-foo-bar.so: libinvoke.so
	ln -s libinvoke.so libinvoke-foo-bar.so 

libinvoke.so: func.c invoke.c
	gcc invoke.c func.c -fPIC -c
	gcc invoke.o func.o -shared -Wl,-soname,libinvoke-foo-bar.so -o libinvoke.so 

clean:
	rm *.o
	rm *.out 
	rm *.so

run:
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/. ./main.out
