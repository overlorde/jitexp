#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <sstream>
#include <string>

namespace internal
{

    // Helper class to impleement DIE

    class FatalLogMessage : public std::basic_ostringstream<char>
    {
    public:
        ~FatalLogMessage();
    };
}

#define DIE internal::FatalLogMessage()

// Simple timer for high-precision timing measurements. Usage:
//
// Timer t;
//.... code to measure
// t.elapsed() returns the number of seconds elapsed since t was created

class Timer
{
public:
    Timer();

    // Returns the number of seconds (as a high precision floating point number)
    // elapsed since the construction of this Timer object

    double elapsed();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> t1_;
};

void parse_command_line(int argc, const char **argv, std::string *bf_file_path, bool *verbose);

#endif /*UTILS_H*/
