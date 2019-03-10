#include <unistd.h>
#include <iostream>
#include <string>
#include "Helpers.h"
#include "Parse.h"

using namespace std;

main (int argc, char** argv) {


  std::string command;
  int running = 1;
  vector<int> running_pids;
  //main input loop. Will be turned into while later.
  while (running != 0) {
    //print prompt
    cout << command_prompt() << endl;
    //get input
    getline(cin, command);
    vector<command_block> input_list = handle_input(command);

    running = execute_commands(input_list, running_pids);
    //execute commands as dictated by input

        //parse input

    //prepare for next loop
  }
}
