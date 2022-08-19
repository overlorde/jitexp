#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "parser.h"
#include "utils.h"

constexpr int MEMORY_SIZE = 30000;

std::vector<size_t> compute_jumptable(const Program &p)
{
    size_t pc = 0;
    size_t program_size = p.instructions.size();
    std::vector<size_t> jumptable(program_size, 0);

    while (pc < program_size)
    {
        char instruction = p.instructions[pc];
        if (instruction == '[')
        {
            int bracket_nesting = 1;
            size_t seek = pc;

            while (bracket_nesting && ++seek < program_size)
            {
                if (p.instructions[seek] == ']')
                {
                    bracket_nesting--;
                }
                else if (p.instructions[seek] == '[')
                {
                    bracket_nesting++;
                }
            }

            if (!bracket_nesting)
            {
                jumptable[pc] = seek;
                jumptable[seek] = pc;
            }
            else
            {
                DIE << "unmatched '[' at pc=" << pc;
            }
        }
        pc++;
    }
    return jumptable;
}

void optInterp(const Program &p, bool verbose)
{

    // Initialize state

    std::vector<uint8_t> memory(MEMORY_SIZE, 0);
    size_t pc = 0;
    size_t dataptr = 0;

    Timer t1;
    std::vector<size_t> jumptable = compute_jumptable(p);

#ifdef BFTRACE
    std::unordered_map(char, size_t) op_exec_count;
#endif

    if (verbose)
    {
        std::cout << "* jumptable [elapsed " << t1.elapsed() << "s]:";
        for (size_t i = 0; i < jumptable.size(); ++i)
        {
            if (jumptable[i])
            {
                std::cout << "[" << i << "]=" << jumptable[i] << " ";
            }
        }
        std::cout << "\n";
    }

    // Pre-caching the program size shouldn't make a difference since p is const
    // but it does with gcc 4.8.4, speeding the interpreter up by 10%
    // It doesn't affect clang 3.7

    size_t program_size = p.instructions.size();

    while (pc < p.instructions.size())
    {
        char instruction = p.instructions[pc];

        switch (instruction)
        {

        case '>':
            dataptr++;
            break;
        case '<':
            dataptr--;
            break;
        case '+':
            memory[dataptr]++;
            break;
        case '-':
            memory[dataptr]--;
            break;
        case '.':
            std::cout.put(memory[dataptr]);
            break;
        case ',':
            memory[dataptr] = std::cin.get();
            break;

        case '[':
            if (memory[dataptr] == 0)
            {
                pc = jumptable[pc];
            }
            break;

        case ']':
            if (memory[dataptr] != 0)
            {
                pc = jumptable[pc];
            }
            break;

        default:
        {
            DIE << "bad char '" << instruction << "' at pc=" << pc;
        }
        }
        pc++;
    }

    // Done running the program. Dump satate if verbose.
    if (verbose)
    {
        std::cout << " pc="
                  << "\n";
        std::cout << " dataptr=" << dataptr << "\n";
        std::cout << "* Memory nonzero locations: \n";

        for (size_t i = 0, pcount = 0; i < memory.size(); ++i)
        {
            if (memory[i])
            {
                std::cout << std::right << "[" << std::setw(3) << std::left
                          << static_cast<int32_t>(memory[i]) << "     ";

                pcount++;

                if (pcount > 0 && pcount % 4 == 0)
                {
                    std::cout << "\n";
                }
            }
        }
        std::cout << "\n";

#ifdef BFTRACE
        std::cout << "* Tracing:\n";
        std::cout.imbue(std::locale(""));
        size_t total = 0;

        for (auto i : op_exec_count)
        {
        std:
            cout << i.first << " --> " << i.second << "\n";
            total += i.second;
        }

        std::cout << ".. Total: " << total << "\n";

#endif
    }
}

int main(int argc, const char **argv)
{
    bool verbose = false;

    std::string bf_file_path;
    parse_command_line(argc, argv, &bf_file_path, &verbose);

    Timer t1;
    std::ifstream file(bf_file_path);

    if (!file)
    {
        DIE << "unable to open file" << bf_file_path;
    }

    Program program = parse_from_stream(file);

    if (verbose)
    {
        std::cout << "[>] Running optInterp:\n";
    }

    Timer t2;
    optInterp(program, verbose);

    if (verbose)
    {
        std::cout << "[<] Done (elapsed: " << t2.elapsed() << "s)\n";
    }

    return 0;
}
