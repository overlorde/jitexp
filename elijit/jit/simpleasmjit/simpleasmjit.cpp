
// using asmjit library

#include <iomanip>
#include <fstream>
#include <iomanip>
#include <stack>
#include <asmjit/asmjit.h>

#include <vector>

#include "parser.h"
#include "utils.h"

#define ASMJIT_STATIC

constexpr int MEMORY_SIZE = 30000;

// This function will be invoked from JITed code; not using putchar directly
// since it can be a macro on some systems, so taking its address is
// problematic.


void myputchar(uint8_t c){
    putchar(c);
}

// ... wrapper for the same reason myputchar

uint8_t mygetchar(){
    return getchar();
}

struct BracketLabels{
   BracketLabels(const asmjit::Label& ol, const asmjit::Label& cl)
       : open_label(ol), close_label(cl){}

   asmjit::Label open_label;
   asmjit::Label close_label;
};

void simpleasmjit(const Program& p, bool verbose){

    // Initialize state
    std::vector<uint8_t> memory(MEMORY_SIZE, 0);

    std::stack<BracketLabels> open_bracket_stack;

    // Initialize asmjit's JIT runtime, code holder and assembler.

    asmjit::JitRuntime jit_runtime;
    asmjit::CodeHolder code;

    code.init(jit_runtime.environment());
    asmjit::x86::Assembler assm(&code);

    // Registers used in the program
    //
    // r13: the pointer
    //rdi :: parameter from the host -- the host passes the address of memory here.
    // here.

    // We pass the data pointer as an argument to the JITed function, so it's
    // expected to be in rdi. Move it to r13.

    asmjit::x86::Gp dataptr = asmjit::x86::r13;
    assm.mov(dataptr, asmjit::x86::rdi);

    for(size_t pc = 0; pc < p.instructions.size(); ++pc){

	char instruction = p.instructions[pc];
	switch(instruction){
	    case '>':
		// inc %r13
		assm.inc(dataptr);
		break;
	    case '<':
		// dec %r13
		assm.dec(dataptr);
		break;
	    case '+':
		// addb $1, 0(%r13)
		assm.add(asmjit::x86::byte_ptr(dataptr), 1);
		break;
	    case '-':
		// subb $1, 0(%r13)
		assm.sub(asmjit::x86::byte_ptr(dataptr), 1);
		break;
	    case '.':
		// call myputchar [dataptr]

		assm.movzx(asmjit::x86::rdi, asmjit::x86::byte_ptr(dataptr));
		assm.call(asmjit::imm(myputchar));
		break;

	    case ',':
		// [dataptr] = call mygetchar

		// Store only the low byte to memory to avoid overwriting unrelated data

		assm.call(asmjit::Imm(mygetchar));
		assm.mov(asmjit::x86::byte_ptr(dataptr), asmjit::x86::al);
		break;

	    case '[':{
			 assm.cmp(asmjit::x86::byte_ptr(dataptr), 0);
			 asmjit::Label open_label = assm.newLabel();
			 asmjit::Label close_label = assm.newLabel();

			 // Jump past the closing ']' if [dataptr] = 0; close_label wasn't
			 // bound yet it will be bound when we handle the matching ']', but
			 // asmjit lets us emit the jump now and will handle the back-patching
			 // later.

			 assm.jz(close_label);

			 // open_label is bound past the jump; all in all, we're emitting:
			 //
			 // cmpb 0(%r13), 0
			 // jz close_label
			 // open_label:
			 // ...

			 assm.bind(open_label);

			 // save both labels on the stack.

			 open_bracket_stack.push(BracketLabels(open_label, close_label));
			 break;

		     }
	    case ']':{
		     if(open_bracket_stack.empty()){
			 DIE << "unmatched closing ']' at pc=" << pc;
		     }

		     BracketLabels labels = open_bracket_stack.top();
		     open_bracket_stack.pop();

		     // cmpb 0(%r13), 0
		     // jnz open_label
 		     // close_label:
		     // ...

		     assm.cmp(asmjit::x86::byte_ptr(dataptr), 0);
		     assm.jnz(labels.open_label);
		     assm.bind(labels.close_label);
		     break;
	    }
		default:
		     {
			 DIE << "bad char '" << instruction << "' at pc=" << pc;
		     }
	}


    }

    assm.ret();
    printf("test1\n");
    // My experimental part starts here.

    using Func = void (*)(uint64_t);
    Func fn;
    asmjit::Error err = jit_runtime.add(&fn, &code);

    //printf("test3\n");


    if(err){
	printf("AsmJit failed: %s\n", asmjit::DebugUtils::errorAsString(err));

    }

    fn((uint64_t)memory.data());
    jit_runtime.release(fn);

    printf("test2\n");
    // My experimental part ends here.



    /*if(assm.isInErrorState()){
	DIE << "asmjit error: " << asmjit::DebugUtils::errorAsString(asm.getLastError());
    }*/


    // Save the emitted code in a vector so we can dump it in verbose mode.
    // Note: The first section is always '.text', so it's safe to just use 0
    // index

    /*
       code.sync();
    asmjit::CodeBuffer& buf = code.getSectionEntry(0)->getBuffer();


    std::vector<uint8_t> emitted_code(buf.getLength());
    memcpy(emitted_code.data(), buf.getData(), buf.getLength());
    */



    // JIT the emitted function
    // JittedFunc is the C++ type for the JIT function emitted by our JIT. The
    // emitted function is callable from C++ and follows the x64 System V ABI.

    /*
       using JittedFunc = void (*)(uint64_t);

    JittedFunc func;

    asmjit::Error err = jit_runtime.add(&func, &code);

    if(err){
	DIE << "error calling jit_runtime.add";
    }

    // Call it, passing the address of memory as a parameter.

    func((uint64_t)memory.data());
    */
    /*if(verbose){
	const char* filename = "/tmp/bjout.bin";
	FILE* outfile = fopen(filename, "wb");

	if(outfile){
	    size_t n = emitted_code.size();
	    if(fwrite(emitted_code.data(), 1, n, outfile) ==  n){
		std::cout << "* emitted code to " << filename << "\n";
	    }
	    fclose(outfile);

	}

	std::cout << "* Memory nonzero locations: \n";

	for(size_t i = 0, pcount = 0; i < memory.size(); ++i){
	    if(memory[i]){
		std::cout << std::right << "[" << std::setw(3) << i
		    << "] = " << std::setw(3) << std::left
		    << static_cast<int32_t>(memory[i]) << "		";
		pcount++;

		if(pcount > 0 && pcount % 4 == 0){
		    std::cout << "\n";
		}
	    }
	}
	std::cout << "\n";
    }*/
}

int main(int argc, const char** argv){
    bool verbose = false;

    std::string bf_file_path;
    parse_command_line(argc, argv, &bf_file_path, &verbose);

    Timer t1;

    std::ifstream file(bf_file_path);
    if(!file){
	DIE << "unable to open file" << bf_file_path;
    }

    Program program = parse_from_stream(file);

    if(verbose){
	std::cout << "Parsing took: " << t1.elapsed()<< "s\n";
	std::cout << "Length of program: "<< program.instructions.size()<<"\n";
	std::cout << "Program:\n" << program.instructions << "\n";
    }

    if(verbose){
	std::cout<<"[>] Running simpleasmjit:\n";
    }

    Timer t2;

    simpleasmjit(program, verbose);

    if(verbose){
	std::cout << "[<] Done (elapsed: "<<t2.elapsed()<<"s)\n";
    }

    return 0;
}
