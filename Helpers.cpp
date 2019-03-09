#include "Helpers.h"

using namespace std;
string command_prompt() {

    //get current dir
    //this block sourced from http://www.codebind.com/cpp-tutorial/c-get-current-directory-linuxwindows/
    char buffer [FILENAME_MAX]; //defined in stdio.h
    getcwd(buffer, FILENAME_MAX);
    string working_dir{buffer};

    //get time
    struct tm* local_time;
    time_t temp_time{time(0)}; //take time int from system clock
    local_time = localtime(&temp_time); //convert to local time
    strftime(buffer, FILENAME_MAX, "%Y-%b-%d %R", local_time); //convert to Y-M-D format, followed by 24-Hr time
    string date_time{buffer};

    getlogin_r(buffer, FILENAME_MAX); //get current username
    string username{buffer};

    string prompt = date_time + " :: " + username + " : " + working_dir;
    return prompt;
}

//returns ints based on whether the main loop should continue
// 0 = end
// 1 = continue
int execute_commands(std::vector<command_block> commands) {

    //special commands, which ignore the rest of the input
    if (commands.at(0).command == "exit" || commands.at(0).command == "quit") {
        return 0; // if first word is exit, then exit the shell
    } else if (commands.at(0).command == "cd") {
        //change dir to second item in input if first is cd
        int error = chdir(commands.at(0).arguments.at(0).c_str());
        if (error == -1)
            perror(commands.at(0).command.c_str());
        return 1;
    }

    //else-wise, execute commands as normal
    bool piping = false;
    bool file_out = false;
    bool background = false;
    vector<pid_t> running_pids;
    vector<char*> argv;

    for (int i = 0; i < commands.size(); i++) {
        background = commands.back().ampersand; //Is this supposed to run in the background?
        command_block active = commands.at(0);

        //const_cast converts a const char* to a char*
        argv.push_back(const_cast<char*>(active.command.c_str()));
        for (int j = 0; j < active.arguments.size(); j++) //make arguments into vector<char*>
            argv.push_back(const_cast<char*>(active.arguments.at(i).c_str()));
        argv.push_back(nullptr); //terminate with nullptr

        pid_t pid = fork();
        if (pid == 0) {
            //child
            char error = execvp(active.command.c_str(), argv.data());
            perror(argv.at(0));
            exit(1); //if we're still here, execution failed: print error message and exit
        } else {
            wait(NULL);
        }

    }
    return 1;
    //probably not actually ready for this yet, but... let's try and plan it out.
    /*
    Loop through every command block in the vector.
    For each, check if it gets input from a file. Also check what comes after it (standard out, file out, or pipe)
    After that, set it up so it can run all of the stuffs in a series!
    Don't worry, ampersand only comes at the very last one, so we don't need to worry too much about that
    File input will only come as the second to last argument of a command, so you can just check there!
    Set pipeline between main and child, then set just move the non-parent end of it? I mean. I think that makes sense to me.
    Idk, I'll have to figure out that part later...
    Output redirection (to file) can only come on the last command as well

    Will also need to handle special cases like cd and cd -
    Need to figure out how to track env variables

    When a command wants to be backgrounded, fork it and then **don't** wait for it
        Prolly just do a check after every other program call or smth? idk if that's feasible

    As for that relative directory thing... I feel like you could probably just *try* it with absolute directory, if fails try elsewhere?
    idk lol

    cd inside of a pipe array doesn't do anything */
}
