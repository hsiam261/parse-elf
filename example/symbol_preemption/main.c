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

void func_DEFAULT(){
        printf("func_DEFAULT redefined in main program, Preempted ==> EXP\n");
}

void func_PROC(){
        printf("func_PROC redefined in main program, Preempted ==> EXP\n");
}
