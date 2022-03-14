#include <stdio.h>

char x='a';
char y='b';

void f();

int main() {
	f();
	printf("%c%c\n",y,x);
	return 0;
}
