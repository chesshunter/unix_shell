#ifndef HELPERS
#define HELPERS
#include "sys/wait.h"
#include "Parse.h"

//return custom command prompt
std::string command_prompt();

int execute_commands(std::vector<command_block> commands);
#endif
