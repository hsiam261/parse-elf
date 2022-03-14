#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	char* error=NULL;
	void* handle = dlopen("./libfoobar.so",RTLD_LAZY);
	
	if(!handle) {
		fprintf(stderr,"%s\n",dlerror());
		exit(1);
	}
	void (*foo)() = dlsym(handle,"foo");
	if((error=dlerror())!=NULL){
		fprintf(stderr,"%s\n",error);
	}
	foo();
	void (*bar)() = dlsym(handle,"bar");
	if((error=dlerror())!=NULL){
		fprintf(stderr,"%s\n",error);
	}
	bar();

	return 0;
}
