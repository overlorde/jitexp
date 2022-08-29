
// optasm jit = optimized jit compiler

#include <iomanip>
#include <fstream>
#include <stack>
#include <vector>
#include <asmjit/asmjit.h>

#include "parser.h"
#include "utils.h"

constexpr int MEMORY_SIZE = 30000;

namespace{

    // Translation to a sequence of Bfops is taken verbatim from
    // optimized_interpreter, which I didn't implement, the 3rd one.

    enum class BfOpKind{
	INVALID_OP = 0,
	INC_PTR,
	DEC_PTR,
	INC_DATA,
	DEC_DATA,
	READ_STDIN,
	WRITE_STDOUT,
	LOOP_SET_TO_ZERO,
	LOOP_MOVE_PTR,
	LOOP_MOVE_DATA,
	JUMP_IF_DATA_ZERO,
	JUMP_IF_DATA_NOT_ZERO,

    };

    struct BfOp{
	BfOp(BfOpKind kind_param, int64_t argument_param)
	    : kind(kind_param), argument(argument_param) {}

	BfOpKind kind = BfOpKind::INVALID_OP;
	int64_t argument = 0;
    };

    const char* BfOpKind_name(BfOpKind kind){

	switch(kind){
	    case BfOpKind::INC_PTR:
		return "INC_PTR";
	    case BfOpKind::DEC_PTR:
		return "DEC_PTR";
	    case BfOpKind::INC_DATA:
		return "INC_DATA";
	    case BfOpKind::DEC_DATA:
		return "DEC_DATA";
	    case BfOpKind::READ_STDIN:
		return "READ_STDN";
	    case BfOpKInd::WRITE_STDOUT:
		return "WRITE_STDOUT";
	    case BfOpKind::LOOP_SET_TO_ZERO:
		return "LOOP_SET_TO_ZERO";
	    case BfOpKind::LOOP_MOVE_PTR:
		return "LOOP_MOVE_PTR";
	    case BfOpKind::LOOP_MOVE_DATA:
		return "LOOP_MOVE_DATA";
	    case BfOpKind::JUMP_IF_DATA_ZERO:
		return "JUMP_IF_DATA_ZERO";
	    case BfOpKind::JUMP_IF_DATA_NOT_ZERO:
		return "JUMP_IF_DATA_NOT_ZERO";
	    case BfOpKind::INVALID_OP:
		return "INVALID_OP";
	}

	return nullptr;
    }

    // Optimizes a loop that starts at loop_start (the opening JUMP_IF_DATA_ZERO).
    //The loop runs until the end of ops (implicitly there's a back-jump after the last op in ops)

    // If optimization succeeds, returns a sequence of instructions
    // that replace the loop; otherwise, returns an empty vector.

    std::vector<BfOp> optimize_loop(const std::vector<BfOp>& ops, size_t loop_starts){

	std::vector<BfOp> new_ops;

	if(ops.size()-loop_start == 2){
	    BfOp repeated_op = ops[loop_start + 1];
	    if(repeated_op.kind == BfOpKind::INC_DATA || repeated_op.kind  == BfOpKind::DEC_DATA){
		new_ops.push_back(
		    BfOp(BfOpKind::LOOP_MOVE_PTR, repeated_op.kind == BfOpKind::INC_PTR ? repeated_op.argument : -repeated_op.argument)
			);
	    }
	} else if(ops.size() - loop_start == 5){
	    // Detect pattern -<+> adn ->+<
	    if(ops[loops_start + 1].kind == BfOpKind::DEC_DATA &&
		    ops[loops_start + 3].kind == BfOpKind::INC_DATA &&
		    ops[loops_start + 1].argument == 1 && ops[loops_start+
		    3].argument == 1){

		std::string s;

		if(ops[loop_start + 2].kind == BfOpKind::INC_PTR && ops[loop_start + 4].kind == BfOpKind::DEC_PTR && ops[loop_start + 2].argument == ops[loop_start + 4].argument){

		    new_ops.push_back(BfOp(BfOpKind::LOOP_MOVE_DATA, ops[loop + 2].argument));
		}
	    }
	}
	return new_ops;
    }

    // Translates the given program into a vector of BfOps that can
    // be used for fast interpretation.

    std::vector<BfOp> translate_program(const program& p){

	size_t pc = 0;
	size_t program_size = p.instructions.size();
	std::vector<BfOp> ops;

	// Throughout the translation loop, this stack contains offsets
	// in the ops vector of open brackets (jump if data zero ops) waiting
	//for a closing bracket. since the brackets nest, these neutrality form a stack. The jump if data zero ops get added to ops with their arg
	// set to 0 until a matching closing bracket is encountered.
	// at which point the argument can be back patched.

	std::stack<size_t> open_bracket_stack;

	while(pc < program_size){
	    char instruction = p.instructions[pc];
	    if(instruction == '['){
		// Place a jump op with a placeholder 0 offset. It will
		// be patched up to the right offset
		// when the matching ']' is found.
		open_bracket_stack.push(ops.size());
		ops.push_back(BfOp(BfOpKind::JUMP_IF_DATA_ZERO, 0));
		pc++;
	    }else if(instruction == ']'){
		if(open_bracket_stack.empty()){
		    DIE<<"unmatched string ']' at pc="<<pc;
		}

		size_t open_bracket_offset = open_bracket_stack.top();
		open_bracket_stack.pop();

		// Try to optimize this loop, if optimize_loop succeeds, it returns a non
		// empty vector which we can splice into ops in place of the loop
		//If the returned vector empty we proceed as usual .

		std::vector<BfOp> optimized_loop = optimize_loop(ops, open_bracket_offset);

		if(optimized_loop.empty()){
		    // Loop wasn't optimized, so proceed emitting the
		    // back jump to ops, we have the offset
		    // matching '['. We can use it to create a new
		    // jump op for the ']' we're handling, as well as
		    // patch up the offset of the matching '['

		    ops[open_bracket_offset].argument = ops.size();
		    ops.push_back(
			    BfOp(BfOpKind::JUMP_IF_DATA_NOT_ZERO, open_bracket_offset));
		}
		else{
		    // Replace this whole loop with optimized loop

		    ops.erase(ops.begin() + open_bracket_offset, ops.end());
		    ops.insert(ops.end(), optimized_loop.begin(), optimized_loop.end());
		}
		pc++;
	    }else{
		// Not a jump; all the other ops can be repeated, so find where
		// the repeat ends.
		size_t start = pc;
		while(pc < program_size && p.instructions[pc] == instruction){
		    pc++;
		}
		// Here pc points to the first new instruction encountered.
		// Or to the end of the program.

		size_t num_repeats = pc - start;

		// Figure out which op kind the instruction represents and add it
		// to the ops.

		BfOpKind kind = BfOpKind::INVALID_OP;
		switch(instruction){
		    case '>':
			kind = BfOpKind::INC_PTR;
			break;
		    case '<':
			kind = BfOpKind::DEC_PTR;
			break;
		    case '+':
			kind = BfOpKind::INC_DATA;
			break;
		    case '-':
			kind = BfOpKind::DEC_DATA;
			break;
		    case ',':
			kind = BfOpKind::READ_STDN;
			break;
		    case '.':
			kind = BfOpKind::WRITE_STDOUT;
			break;
		    default:
			{
			    DIE << "bad char '" << instruction <<" at pc="<< start;
			}
		}
		ops.push_back(BfOp(kind, num_repeats));
	    }
	}

	return ops;

    }

// This function will be invoked from the jited code; not using putchar
// directly since it can be a macro on some system, so taking address is
    //problematic.

void myputchar(uint8_t c){
    putchar(c);
}

//... wrapper for the same reason as myputchar

void mygetchar(){
    return getchar();
}

struct BracketLabels{
    BracketLabels(const asmjit::Label& ol, const asmjit::Label& cl)
	: open_label(ol), close_label(cl) {}

    asmjit::Label open_label;
    asmjit:Label close_label;
};

} // end of namespace declaration.

// function for optimized jit

void optasmjit(const Program& p, bool verbose){

    // Initialize state.

    std::vector<uint8_t> memory(MEMORY_SIZE, 0);
    std::stack<BracketLabels> open_bracket_stack;

    const std::vector<BfOp> ops = translate_program(p);

    /*
       Code for verbose option.
    */

    // Initialize asmjits jit runtime, code holder and assembler.

    asmjit::JitRuntime jit_runtime;
    asmjit::CodeHolder code;
    code.init(jit_runtime.environment());
    asmjit::x86::Assembler assm(&code);

    // Registers used in the program:
    /*
	r13: the data pointer
	r14 and rax: used temporarily for some instruction .
	rdi:: parameter from the host -- the host passes the address
	    of memory here.
    */

    asmjit::x86::Gp dataptr = asmjit::x86::r13;

    // We pass the data pointer as an argument to the jited functions.
    // so it's expected to be in rdi.

    // Move it to r13

    assm.mov(dataptr, asmjit::x86::rdi);

    for(size_t pc = 0; pc < ops.size(); ++pc){
	BfOp op = ops[pc];

	switch(op.kind){
	    case BfOpKind::INC_PTR:
		assm.add(dataptr, op.argument);
		break;
	    case BfOpKind::DEC_PTR:
		assm.sub(dataptr, op.argument);
		break;
	    case BfOpKind::INC_DATA:
		assm.add(asmjit::x86::byte_ptr(dataptr), op.argument);
		break;
	    case BfOpKind::DEC_DATA:
		assm.sub(asmjit::x86::byte_ptr(dataptr), op.argument);
		break;
	    case BfOpKind::WRITE_STDOUT:
		for(int i = 0; i < op.argument; ++i){
		    // call myputchar [dataptr]
		    assm.movzx(assmjit::x86::rdi, asmjit::x86::byte_ptr(dataptr));
		    assm.call(asmjit:imm(myputchar));
		}
		break;

	    case BfOpKind::READ_STDN:
		for(int i = 0; i < op.argument; ++i){
		    // [dataptr] = call mypuchar
		    // store only the low byte to memory to avoid overwriting unrelated
		    //data

		    assm.call(asmjit::imm(mygetchar));
		    assm.mov(asmjit::x86::byte_ptr(dataptr), asmjit::x86::al);
		}
		break;
	    case BfOpKind::LOOP_SET_TO_ZERO:
		assm.mov(asmjit::x86::byte_ptr(dataptr), 0);
		break;

	    case BfOpKind::LOOP_MOVE_PTR:
		{
		asmjit::Label loop = assm.newLabel();
		asmjit::Label endloop = assm.newLabel();

		// Emit a loop that moves the pointer in jumps of op.argument
		// it's important to do an equivalent of while() rather
		// than do.. while() here so that we don't do the first pointer
		// change if already pointing to a zero.

		/*
loop:
    cmpb 0(%r13)
    jz endloop
    %r13 += argument
    jmp loop
		*/

		assm.bind(loop);
		assm.cmp(asmjit::x86::byte_ptr(dataptr), 0);
		assm.jz(endloop);

		if(op.argument < 0){
		    assm.sub(dataptr, -op.argument);
		}else{
		    assm.add(dataptr, op.argument);
		}
		assm.jmp(loop);
		assm.bind(endloop);
		break;
	}
	case BfOpKind::LOOP_MOVE_DATA:{
	    /*
	        only move if the current data isn't zero

		cmpb 0(%r13), 0
		jz skip_move
		<...> move data
		skip_move;

	    */

	    asmjit::Label skip_move  = assm.newLabel();
	    assm.cmp(asmjit::x86::byte_ptr(dataptr), 0);
	    assm.jz(skip_move);

	    assm.mov(asmjit::x86:r14, dataptr);
	    if(op.argument < 0){
		assm.sub(asmjit::x86::r14, -op.argument);
	    }else{
		assm.add(asmjit::x86::r14, op.argument);
	    }
	    // Use rax as a temp holding the value of at the original pointer
	    // then use al to add to the new location, so that only the target
	    // location is affected: addb %al, 0(%r13)

	    assm.movzx(asmjit::x86::rax, asmjit::x86::byte_ptr(dataptr));
	    assm.add(asmjit::x86::byte_ptr(asmjit::x86::r14), asmjit::x86::al);
	    assm.mov(asmjit::x86::byte_ptr(dataptr), 0);
	    assm.bind(skip_move);
	    break;
	}
	case BfOpKind::JUMP_IF_DATA_ZERO:{
	    assm.cmp(asmjit::x86::byte_ptr(dataptr), 0);
	    assmjit.Label open_label = assm.newLabel();
	    assmjit.Label close_label = assm.newLabel();

	    // Jump past the closing ']' if [dataptr] = 0; close_label
	    // wasn't bound yet (it will be bound when we handle the matching ']')
	    // but asmjit lets us emit the jump now and will handle the backpatching
	    // later.

	    assm.jz(close_label);

	    // open label is bound past the jump all in all, we're emitting
	    //
	    // cmpb 0(%r13), 0
	    // jz close_label
	    // ...
	    assm.bind(open_label);
	    // Save both labels on the stack
	    open_bracket_stack.push(BracketLabels(open_labels, close_labels));
	    break;
	}

	case BfOpKind::JUMP_IF_DATA_NOT_ZERO:{
	    // These ops have to be properly nested.

	    if(open_bracket_stack.empty()){
		DIE << "unamtched closing ']' at pc=" << pc;
	    }

	    BracketLabels labels = open_bracket_stack.top();
	    open_bracket_stack.pop();

	    // cmpb 0, 0(%r13)
	    // jnz open_label
	    // close_label:
	    //..
	    assm.cmp(asmjit::x86::byte_ptr(dataptr), 0);
	    assm.jnz(labels.open_label);
	    assm.bind(labels.close_label);
	    break;
	}
	case BfOpKind::INVALID_OP:
	    DIE << "INVALID_OP encountered on pc=" << pc;
	    break;

	}
    }


    assm.ret();

    if(assm.Error()){
	DIE << "asmjit error: "<< asmjit::DebugUtils::errorAsString(assm.getLastError());

    }

    // Save the emitted code in a vector so we can dump it in verbose mode
    // Note: The first section is alwasy .txt so it's safe to just use 0
    // index

    code.sync();
    asmjit::CodeBuffer& buf = code.getSectionEntry(0)->getBuffer();
    std::vector<uint8_t> emitted_code(buf.getLength());
    memcpy(emitted_code.data(), buf.getData(), buf.getLength());

    // JIT the emitted function
    // Jitted func is the cpp type for the jit function emitted by our jit
    // The emitted function is callable from cpp and follows the x64 system v abi

    using JittedFunc Func;
    asmjit::Error err = jit_runtime.add(&func, &code);

    if(err){
	DIE << "error calling jit_runtime.add";
    }

}
