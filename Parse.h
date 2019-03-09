#ifndef PARSE
#define PARSE
#include <stdio.h> //defines FILENAME_MAX
#include <iostream>
#include <ctime>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>
//special commands (cd and exit) will be handled seperately, so don't worry about them
struct command_block {
    bool ampersand; //does this have an ampersand at the end?
    std::string command;
    std::vector<std::string> arguments; //arguments for command.
    char after = 'a'; //is the next thing a pipe? a redirect to an output file?
};

std::vector<command_block> handle_input(std::string input_string);

#endif
