main.out: main.c libfoo.so
	gcc -o main.out main.c -L./ -lfoo -Wl,-rpath,./ 


libfoo.so: foo.o
	gcc --shared foo.o -o libfoo.so

foo.o: foo.c
	gcc -c -o foo.o foo.c -fPIC

clean:
	rm *.o *.so *.out
