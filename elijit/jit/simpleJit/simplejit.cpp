#include <cstdio>
#include <fstream>
#include <iomanip>
#include <stack>

#include "jit_uitls.h"
#include "parser.h"
#include "utils.h"

contexpr int MEMORY_SIZE = 30000;

void simplejit(const Program& p, bool verbose){
    // Initialize state.

    std::vector<uint8_t> memory(MEMORY_SIZE, 0);

    // Registers used in the program;
    //
    // r13 the data pointer -- contains the address of memory.data()

    //rax, rdi, rsi, rdx: used for making system calls, per the ABI

    CodeEmitter
}
