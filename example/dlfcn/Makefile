main.out: main.c libfoobar.so
	gcc -o main.out main.c -rdynamic -ldl 

run: main.out 
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/. ./main.out


libfoobar.so: foo.o bar.o
	gcc -o libfoobar.so -shared foo.o bar.o

foo.o: foo.c
	gcc -o foo.o foo.c -c -fPIC

bar.o: bar.c
	gcc -o bar.o bar.c -c -fPIC


clean:
	rm *.o *.out *so
