#include <stdio.h>

extern void func_DEFAULT();
extern void func_PROC();
extern void invoke();
extern int x;
int x = 1000;

int main(){
        invoke();
	printf("%d\n",x);
        return 0;
}


