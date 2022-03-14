#include <stdio.h>
int x = 25;
extern int y;

extern void foo();

int main() {
	foo();
	foo();
	foo();
	foo();
	foo();
	printf("%d\n",x);
	printf("%d\n",y);
}
