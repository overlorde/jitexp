#include "parser.h"
#include "utils.h"

#include <iostream>
#include <sstream>

Program parse_from_stream(std::istream &stream)
{
    Program program;

    for (std::string line; std::getline(stream, line);)
    {
        for (auto c : line)
        {
            if (c == '>' || c == '<' || c == '+' || c == '-' || c == '.' || c == ',' || c == '[' || c == ']')
            {
                program.instructions.push_back(c);
            }
        }
    }

    return program;
}