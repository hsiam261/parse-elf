main.out: main.c libfoo.a
	gcc --static -o main.out main.c libfoo.a

main2.out: main2.c libfoo.a
	gcc --static -o main2.out main2.c libfoo.a

libfoo.a: foo.o bar.o
	ar rcs libfoo.a foo.o bar.o

foo.o: foo.c
	gcc -c foo.c -o foo.o --static

bar.o: bar.c
	gcc -c bar.c -o bar.o --static 


dumb: main.c libfoo.a
	gcc --static -o main.out libfoo.a main.c
clean:
	rm *.o *.a *.out 

