#include <stdio.h>

extern int v4;

int v0;
int v1 = 7;
int v2 = 8;

static int v3 = 9;



static void hello() {
	printf("Hello World\n");
}

void f() {
	static int x=0;
	printf("%d %d in f\n",x,v4);
	x++;
	hello();
}


void g() {
	static int x=0;
	printf("%d in g\n",x);
	x++;
}


