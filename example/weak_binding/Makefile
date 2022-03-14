main.out: main.o a.o 
	gcc -std=c89 main.o a.o -o main.out 

main.o: main.c
	gcc -std=c89 main.c -c -o main.o 

a.o: a.c
	gcc -std=c89 a.c -c -o a.o 

clean:
	rm *.o *.out 


