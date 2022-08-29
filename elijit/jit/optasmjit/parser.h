#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>

struct Program
{
    std::string instructions;
};

Program parse_from_stream(std::istream &stream);

#endif /*PARSER_H*/