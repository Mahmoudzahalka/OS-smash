#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char* argv[]) {
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR)
    {
        perror("smash error: signal failed");  //todo: may need to change it
    }
    //TODO: setup sig alarm handler

    SmallShell& smash = SmallShell::getInstance();
        // for (int i = 0; i < 100; ++i) {
        //     smash.jobsList.jobsListVector[i]=nullptr;
        // }
    char currWD[1024];
    getcwd(currWD,sizeof(currWD)); //check fail
    smash.currentWorkingDirectory=currWD;

    while(true) {
        //kill(getpid(),SIGINT);
        std::cout << smash.shellPrompt << "> "; //TODO : print the shellrpmpt of the shell object V
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        //std::cout << "ala zeby";
        if(cmd_line=="")   //TODO more cases like this ??
            continue;
        smash.executeCommand(cmd_line.c_str());
        //stdout
        //perror("smash error: failed to set ctrl-C handler");
    }
    return 0;
}