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

    string prompt = date_time + "::" + username + ":" + working_dir;
    return prompt;
}

//returns ints based on whether the main loop should continue
// 0 = end
// 1 = continue
int execute_commands(std::vector<command_block>& commands, std::vector<int>& running_pids) {
    string static previous_path;
    //first things first, check on children
    for (int i = 0; i < running_pids.size(); i++) {
        int status;
        waitpid(running_pids.at(i), &status, WNOHANG);
        if ( WIFEXITED(status)) {
            running_pids.erase(running_pids.begin() + i);
        }
    }

    //special commands, which ignore the rest of the input
    if (commands.at(0).command == "exit" || commands.at(0).command == "quit") {
        return 0; // if first word is exit, then exit the shell
    } else if (commands.at(0).command == "cd") {
        //change dir to second item in input if first is cd
        char buffer [FILENAME_MAX]; //defined in stdio.h
        getcwd(buffer, FILENAME_MAX);
        string temp_path = previous_path;
        previous_path = buffer;

        string new_path;
        if (commands.at(0).arguments.at(0) == "-")
            new_path = temp_path;
        else
            new_path = commands.at(0).arguments.at(0).c_str();
        int error = chdir(new_path.c_str());

        if (error == -1) {
            previous_path = temp_path;
            perror(new_path.c_str());
        }
        return 1;
    }

    //else-wise, execute commands as normal
    bool piping = false;
    bool file_out = false;
    bool background = commands.back().ampersand;
    vector<char*> argv;

    int in_next = dup(0); //create room for storing the next iteration's input source
    int out_desc = dup(1); //this iteration's output
    int in_desc = dup(0); //this iteration's input
    int past_pipe = dup(0); //almost certainly all of these aren't needed. but... meh.

    if (commands.at(0).arguments.size() > 1 && (*(commands.at(0).arguments.end() - 2) == "<")) {
        //if first block takes input from a file
        int fd_in = open(commands.at(0).arguments.back().c_str(), O_RDONLY);
        dup2(fd_in, in_next);
    }

    for (int i = 0; i < commands.size(); i++) {
        int pipe_fds[2];
        pipe(pipe_fds); //create pipe at wherever

        command_block active = commands.at(i);

        //handle I/O redirection
        dup2(in_next, in_desc); //copy stored in_source to this iteration's in_desc
        if (active.after == '|') {
            dup2(pipe_fds[1], out_desc); //copy pipeout to stdout
            dup2(pipe_fds[0], in_next); //pipein -> next input's stdin
        } else if (active.after == '>') {
            //open file, assign this iteration's output to that
            string file_name;
            try { file_name = commands.at(i+1).command;}
            catch (out_of_range e) {file_name = " ";}
            int file_fd = open(file_name.c_str(), O_CREAT|O_WRONLY, S_IRUSR | S_IWUSR);
            dup2(file_fd, out_desc);
            dup2(0, in_next); //copy stdin to next iteration's std. Which... shouldn't do anything, but still
        } else {
            dup2(1, out_desc);
            dup2(0, in_next);
        }

        //convert arguments to vector<char*>
        string name = active.command;
        argv.push_back(const_cast<char*>(name.c_str()));
        for (int j = 0; j < active.arguments.size(); j++)
            argv.push_back(const_cast<char*>(active.arguments.at(j).c_str()));
        argv.push_back((char*)NULL); //terminate with nullptr

        pid_t pid = fork();
        if (pid == 0) {
            //child: set file descriptions to correct values, execute program.
            dup2(out_desc, 1);
            dup2(in_desc, 0);
            close(pipe_fds[0]);
            char error = execvp(active.command.c_str(), argv.data());
            perror(argv.at(0));
            exit(1); //if we're still here, execution failed: print error message and exit
        } else {
            //parent: if background, add to running list, else wait for end
            close(pipe_fds[1]);
            if (background) {
                running_pids.push_back(pid);
            } else {
                wait(&pid);
            }
        }

        //resetting things
        argv = {};
    }
    return 1;
}
