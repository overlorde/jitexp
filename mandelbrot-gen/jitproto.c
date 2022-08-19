//jitproto.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef long(*fn)(long);

fn compile_identity(void){
    char *memory = mmap(NULL,
			4096,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0);

    if(memory == MAP_FAILED){
	perror("failed to allocate memory.");
	exit(1);
    }

    int i = 0;

    memory[i++] = 0x48;
    memory[i++] = 0x8b;
    memory[i++] = 0xc7;

    // ret
    memory[i++] = 0xc3;

    return (fn) memory;
}

int main(){
    fn f = compile_identity();
    int i;
    for(i = 0; i < 10; ++i){
	printf("f(%d) = %ld\n",i, (*f)(i));
    }
    munmap(f, 4096);
    return 0;
}

