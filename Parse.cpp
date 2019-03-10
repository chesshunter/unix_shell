#include "Parse.h"

using namespace std;
/*split into different commands, and then also maybe the output.  (split on |, and possibly >. Careful of strings!)
split each individual command into its various sub-pieces: handle tokens, arguments, and modification operators
    Store each command as a new object type, then execute them all sequentially?


/*
be able to handle strings that contain special symbols (&, |, >, <, all that)
Split based on strings first (most practical way to build around strings)
    Oo... what if instead we replaced all symbols of interest inside strings to other things
    Then transmuted them back after we'd done the other processing? Wouldn't that be neat...
    Probably stupid though, lmao.
Then split based on pipes
Then find symbol stuff

//or... split based on | and >, then split each command internally.
//Ampersand can only be at end of the input!
//THEREFORE: check the last command_block if it's ampersanded.

Use WNOHANG flag on wait() to check if a child process has ended.
*/

//return all that good stuff... yeah...
std::vector<command_block> handle_input(string input_string) {
    auto it = input_string.begin();
    bool is_string = false;
    command_block out_block;
    out_block.ampersand = false;
    vector<command_block> out_vec;
    string working = "";

    while (it != input_string.end()) {
        if (*it == '\"' || *it == '\'') { //handle strings! yay!
            is_string = !is_string; //flip it
        } else if (is_string) {
            working = working + *it; //if we're in a string, then just add the next char.
        } else if (*it == '&') {
            out_block.ampersand = true;
        } else if (*it == '>' || *it == '|') {
            //ending operators! Time to end this block!
            if (out_block.command.size() > 0) {
                out_block.after = *it;
                out_vec.push_back(out_block);
                out_block = command_block();
                out_block.ampersand = false;
            }
        } else if (*it == ' ' || it == input_string.end() - 1) {
            //reached a new area, break and slot into the block.
            if (*it != ' ') //if this is the last char, go ahead and insert it.
                working = working + *it;
            if (out_block.command.size() == 0 && working.size() > 0) {
                out_block.command = working;
            }
            else if (out_block.command.size() > 0 && working.size() > 0) {
                out_block.arguments.push_back(working);
            }
            working = "";
        } else {
            //nothing interesting: just add this to the pile
            working = working + *it;
        }
        it += 1;
    }

    if (out_block.command.size() > 0) {
        out_vec.push_back(out_block);
    }

    return out_vec;

}
