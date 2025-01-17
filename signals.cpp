#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

//here may need to change the exit(1)
#define DO_SYS( syscall, syscallName ) \
do { /* safely invoke a system call */ \
 if( (syscall) == -1 ) { \
 perror( "smash error: " #syscallName" failed" ); \
   }            \
 }                       \
while( 0 ) \


void ctrlCHandler(int sig_num) {
  // TODO: Add your implementation
    std::cout << "smash: got ctrl-C" << std::endl;           //add new 13.2 9:33
    SmallShell& tempSmash = SmallShell::getInstance();
    if(tempSmash.currentFgProcess!=0)
    {
        //std::cout << "smash: got ctrl-C" << std::endl;    // new 13.2 9:33
        DO_SYS(kill(tempSmash.currentFgProcess,9), kill);
        std::cout << "smash: process "<< tempSmash.currentFgProcess <<" was killed" << std::endl;
    }
    //std::cout << "smash: process "<< tempSmash.currentFgProcess <<" was killed" << std::endl;
    tempSmash.currentFgProcess=0; //TODO: make sure its updated everywhere
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

