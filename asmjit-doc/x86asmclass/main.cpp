#include <asmjit/x86.h>
#include <stdio.h>

using namespace asmjit;

// Signature of the generated function

typedef int (*SumFunc)(const int* arr, size_t count);

int main(){

    JitRuntime rt; // create a runtime specialized for JIT
    CodeHolder code;	 //Create a codeholder

    code.init(rt.environment());	// Initialize code to match the JIT environment

    x86::Assembler a(&code);	// Create and attach x86::Assembler to code

    // Decide between 32-bit CDECL, WIN64, and SysV64 calling convention

    // 32-bit - passed all arguments by stack.
    // UNIX64 - passes  first 6 args by RDI, RSI, RCX, RDX and R9.

    x86::Gp arr, cnt;
    x86::Gp sum = x86::eax; // Use eax as sum as it's a return register

    if(ASMJIT_ARCH_BITS == 64){
    #if defined(_WIN32)
	arr = x86::rcx; // First arg (array ptr)
	cnt = x86::rdx; // Second arg (number of elements)
    #else
	arr = x86::rdi; // First argument (array ptr)
	cnt = x86::rsi; // Second argument (number of elements)
    #endif
    }
    else{
	arr = x86::edx;  //  Use edx to hold the array pointer
	cnt = x86::ecx;  // Use ecx to hold the counter
	// Fetch the first and second arguments from [esp + 4] and [esp+8]

	a.mov(arr, x86::ptr(x86::esp, 4));
	a.mov(cnt, x86::ptr(x86::esp, 8));

    }

    // To construct the loop we need some labels.

    Label Loop = a.newLabel();
    Label Exit = a.newLabel();

    a.xor_(sum, sum); 	// Clear 'sum' register (shorter than mov)
    a.test(cnt, cnt); 	// Border case
    a.jz(Exit); 	// If 'cnt' is zero jump to "exit" now

    a.bind(Loop);	// Start of a loop iteration
    a.add(sum, x86::dword_ptr(arr)); 	// Add int at [arr] to 'sum'
    a.add(arr, 4); // Increment arr pointer
    a.dec(cnt); // Decrement arr pointer
    a.jnz(Loop); // If not zero jump to Loop

    a.bind(Exit);  // Exit to handle the border case
    a.ret(); 	// Return from function ('sum' == 'eax')

    // x86::Assembler is no longer needed from now on

    SumFunc fn;
    Error err = rt.add(&fn, &code); // Add the generated code to the runtime

    if(err) return 1; // Handle a possible error returned by asmjit

    static const int array[6] = {4, 8, 9, 2, 4, 7};

    int result = fn(array, 6); // Execute the generated code
    printf("%d\n", result); // Print sum of array (108)

    rt.release(fn); // Explicitly remove the function from the runtime
    return 0; // Everythin successful

}
