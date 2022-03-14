#include <stdio.h>

int z;

void bar() {
	static int c=1;
	c++;
	z=c;
	printf("%d bar\n",z);
}
