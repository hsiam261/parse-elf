
main.out: a.o main.o 
	gcc -o main.out main.o a.o 
	cp main.out main.out.stripped
	sstrip main.out.stripped

a.o: a.c
	gcc -c a.c -o a.o 

main.o: main.c
	gcc -c main.c -o main.o

clean:
	rm *.o *.out *.stripped
