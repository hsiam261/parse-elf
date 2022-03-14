#include <stdio.h>

extern int v1;
extern int v2;

int v4;

void f();
void g();

int main() {
	printf("%d %d\n",v1,v2);
	f();
	g();
	f();
	f();
	g();
	printf("%d %d\n",v1,v2);
}
