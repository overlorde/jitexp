#ifndef JIT_UTILS_H
#define JIT_UTILS_H

#include <cstddef>
#include <cstdint>
#include <vector>


class JitProgram{
public:
    JitProgram(const std::vector<uint8_t>& code);
    ~JitProgram();

    // Getting the pointer to program memory. This pointer is valid only as long
    // the JitProgram object is alive.

    void* program_memory(){
	return program_memory_;
    }

    size_t program_size(){
	return program_size_;
    }

private:
    void* program_memory_ = nullptr;
    size_t programZ_size_ = 0;
};

class CodeEmitter{
    public:
	CodeEmitter() = default;

	void EmitByte(uint8_t v);

	// Emits a sequence of consecutive bytes.

	void EmitBytes(std::initializer_list<uint8_t> seq);

	void EmitUint32(uint32_t v);
	void EmitUint64(uint64_t v);

	// Replaces the byte at "offset" with 'v'. Assumes offset < size()

	void ReplaceByteAtOffset(size_t offset, uint8_t v);

	// Replaces the 32-bit word at 'offset' with 'v'. Assume offset + 3 < size().

	void ReplaceUint32AtOffset(size_t offset, uint32_t v);

	size_t size() const{
	    return code_.size();
	}


	const std::vector<uint8_t>& code() const{
	    return code_;
	}

    private:
	std::vector<uint8_t> code_;
};

// Compute a 32-bit relative offset for pc-relative jumps. Given an address to
// jump *from* and an address to jump *to*, returns the 4-byte integer that has
// to be encoded into the offset field of a jmp instruction. The user is
// expected to adjust jump addresses before passing them to this funciton (for example
// taking into account that a jump offset is computed from after the jump instruction itself)
// This is very specific to x86 architecture.

uint32_t compute_relative_32bit_offset(size_t jump_from, size_t jump_to);


#endif

