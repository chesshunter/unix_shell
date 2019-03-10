#ifndef HELPERS
#define HELPERS
#include "sys/wait.h"
#include "Parse.h"
#include <fcntl.h>
#include <sys/stat.h>

//return custom command prompt
std::string command_prompt();

int execute_commands(std::vector<command_block>& commands, std::vector<int>& running_pids);
#endif
