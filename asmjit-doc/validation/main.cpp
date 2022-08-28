#include <asmjit/x86.h>
#include <stdio.h>

using namespace asmjit;

int main(int argc, char* argv[]){
    /*
       Create a runtime specialized for JIT.
       Create a codeholder.
    */

    JitRuntime rt;
    CodeHolder code;

    /*
       Init code to match the JIT environment.
       Create and attach x86::Assembler to code.
    */

    code.init(rt.environment());
    x86::Assembler a(&code);

    // Enable strict validation.
    a.addDiagnosticOptions(DiagnosticOptions::kValidateAssembler);

    // Try to encode invalid or ill-formed instructions.
    Error err;

    // Invalid instruction.
    err = a.mov(x86::eax, x86::al);
    printf("Status: %s\n", DebugUtils::errorAsString(err));

    // Invalid instruction.
    err = a.emit(x86::Inst::kIdMovss, x86::eax, x86::xmm0);
    printf("Status: %s\n", DebugUtils::errorAsString(err));

    // Ambiguous operand size - the pointer requires size.
    err = a.inc(x86::ptr(x86::rax), 1);
    printf("Status: %s\n", DebugUtils::errorAsString(err));

    return 0;
}
