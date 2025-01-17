#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include <string>
#include <fcntl.h>
#include <cerrno>



const std::string WHITESPACE = " \n\r\t\f\v";

using namespace std;

#define PATH_LENGTH 1000

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif



//here may need to change the exit(1)
#define DO_SYS( syscall, syscallName ) do {  \
 if( (syscall) == -1 ) { \
 perror("smash error: " #syscallName " failed"); \
  }            \
 }                       \
while( 0 ) 

//exit(1);  }          


BuiltInCommand::BuiltInCommand(const char* cmd_line) : Command(cmd_line){}
QuitCommand::QuitCommand(const char* cmd_line, JobsList* jobs) : BuiltInCommand(cmd_line){}


string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}


//smallShell static member initializations check these if correct
std::string SmallShell::lastWorkingDirectory= ""; //TODO: empty string
std::string SmallShell::shellPrompt= "smash";
pid_t SmallShell::currentFgProcess =0;
// JobsList SmallShell::jobsList;
// JobsList::JobEntry* JobsList::jobsListVector[100] = { nullptr };
std::string SmallShell::currentWorkingDirectory=""; //what to intilize to check now 







//-------------------------------------- imported


char **init_args(const char *cmd_line, int *num_of_args) {
    char **args = (char **) malloc(COMMAND_MAX_ARGS * sizeof(char **));
    if (!args)
        return nullptr;
    for (int i = 0; i < COMMAND_MAX_ARGS; i++)
        args[i] = nullptr;
    int num = _parseCommandLine(cmd_line, args);
    if (num == -1)
        args = nullptr;
    *num_of_args = num;
    return args;
}

void free_args(char **args, int arg_num) {
    if (!args)
        return;

    for (int i = 0; i < arg_num; i++) {
        if (args[i])
            free(args[i]);
    }
    free(args);
}

//-------------------------------------------------------------------------------------- imported end

void bubbleSortWithId(JobsList::JobEntry** arr,int size) {
    int n = size;
    bool swapped; 

    for (int i = 0; i < n - 1; i++) {
        swapped = false;

        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j]->jobId > arr[j + 1]->jobId) {                //JobEntry* //swap check
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        // If no two elements were swapped by inner loop, the array is already sorted
        if (!swapped) {
            break;
        }
    }
}





bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() {
// TODO: add your implementation
}

// SmallShell::~SmallShell() { //default for now 
// // TODO: add your implementation
// }

/*
 * COMMAND = "showpid>txt1
 * new str = "showpid txt1"
 * args = init_args(new str)
 *
 */

//extract filepath
char* read_until_whitespace(const char* str) {
    int length = 0;
    const char* temp = str;

    // Calculate the length of the substring until whitespace or end of string
    while (*temp != '\0' && *temp != ' ' && *temp != '\t' && *temp != '\n' && *temp != '\r') {
        length++;
        temp++;
    }

    // Allocate memory for the new string
    char* substring = (char*)malloc((length + 1) * sizeof(char));
    if (substring == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Copy the substring into the new string
    strncpy(substring, str, length);
    substring[length] = '\0'; // Null-terminate the new string

    return substring;

    // int length = 0;
    // const char* temp = str;

    // // Calculate the length of the substring until whitespace or end of string
    // while (*temp == ' ' || *temp == '\t' || *temp == '\n' || *temp == '\r') {
    //     length++;
    //     temp++;
    // }
    // int len = 0;
    // while (*temp != '\0' && *temp != ' ' && *temp != '\t' && *temp != '\n' && *temp != '\r') {
    //     length++;
    //     temp++;
    //     len++;
    // }

    // char* substring = (char*)malloc((len + 1) * sizeof(char));
    // if (substring == NULL) {
    //     fprintf(stderr, "Memory allocation failed\n");
    //     exit(1);
    // }
    // strncpy(substring, str+length-len, len);
    // substring[length+len+1] = '\0'; // Null-terminate the new string

    // return substring;
}


    

    
//extract file path done




//case : command line = "showpid >";


//check for IO redirection in craete command
bool containsOverWriteIO(const char* str) {
    while (*str) {
        if (*str == '>' && *(str+1)=='>') 
        {
            return false;
        }
        if (*str == '>' && *(str+1)!='>') {
            //filePath = read_until_whitespace(((_trim(str+1)).c_str())); //
            return true;
        }
        ++str;
    }
    return false;
}

//same case as above check
bool containsAppendIO(const char* str) {
    while (*str) {
        if (*str == '>' &&  *(str+1) == '>') { //TODO: won't leak
            //filePath = read_until_whitespace(((_trim(str+2)).c_str())); //CHECK
            return true;
        }
        ++str;
    }
    return false;
}

const char* IOcommandLine(const char* cmdLine)
{
    int length= strlen(cmdLine)+1;
    char* newCommandLine = (char*) malloc(length*sizeof(char)); //check here
    int i=0;
    for(; i<length-1; ++i)
    {
        if(cmdLine[i]=='>')
        {
            newCommandLine[i]=' ';
            continue;
        }
        newCommandLine[i]=cmdLine[i];
    }
    newCommandLine[i]='\0';
    return newCommandLine;
}

int changeFD(const char* filePath, bool append, bool overwrite)
{
    int targetFD;
    int isExsitFile = access(filePath, F_OK);
    if(append && !(overwrite))  //changed here in debugging 11/2 because of macro expansion
    {
        if (isExsitFile != -1)
        {
           targetFD = open(filePath, O_APPEND | O_WRONLY); //check open for wwrite 11/2
           //targetFD = open(filePath, O_RDWR | O_APPEND); //last debug shouldnt be like
           if(targetFD==-1)
           {
               perror("smash error: open failed");
               return -1; //syscall failed check
           }
        }
       else
       {
           targetFD = open(filePath, O_APPEND | O_WRONLY | O_CREAT, 0644); //check open for wwrite 11/2
           //targetFD = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 0644);
           if(targetFD==-1)
           {
               perror("smash error: open failed");
               return -1; //syscall failed check
           }
       }
    }
       // targetFD = DO_SYS(open(filePath, O_APPEND), open);
    if(overwrite && !(append))
    {
        if(isExsitFile != -1)
        {
            targetFD = open(filePath,O_WRONLY | O_TRUNC);
            //targetFD = open(filePath,O_RDWR | O_TRUNC);
            if(targetFD==-1)
            {
               perror("smash error: open failed");
               return -1; //syscall failed check
            }
        }
        else
        {
            targetFD = open(filePath,O_WRONLY | O_CREAT | O_TRUNC, 0644);
            //targetFD = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 0644);
            if(targetFD==-1)
            {
               perror("smash error: open failed");
               return -1; //syscall failed check
            }
        }

    }
       // targetFD = DO_SYS(open(filePath), open);
    int newSTD ; //= DO_SYS(dup(1),dup);
    newSTD = dup(1);
    if(newSTD == -1)
    {
        perror("smash error: dup failed");
        return -1; //syscall failed check

    }
    int i; // = DO_SYS(dup2(targetFD,1), dup2); //closes newfd alone
    i = dup2(targetFD,1);
    if ( i==-1)
    {
        perror("smash error: dup failed");
        return -1; //syscall failed check
    }
    return newSTD;
}

void resetSTD(int stdFD)
{
    // int i= DO_SYS(dup(1),dup);
    // DO_SYS(dup2(stdFD,1), dup2);
    // DO_SYS(close(i),close);
    int i;
    i = dup(1);
    if( i ==-1)
    {
        perror("smash error: dup failed");
        return;
    }
    int j;
    j = dup2(stdFD,1);
    if( j ==-1)
    {
        perror("smash error: dup2 failed");
        return;
    }
    j = close(i);
        if( j ==-1)
    {
        perror("smash error: close failed");
        return;
    }



}



/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
	// For example:
    //TODO : FOR built in commands "pwd" and "pwd&" "pwd &" are the same

  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));



  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line); 
  }
  else if (firstWord.compare("chprompt") == 0) {
    return new ChpromptCommand(cmd_line);
  }
  else if (firstWord.compare("cd") == 0) {
    return new ChangeDirCommand(cmd_line,nullptr);     // DUMMY SADY
  }
  else if (firstWord.compare("jobs") == 0) {      // nullptr ?
    return new JobsCommand(cmd_line,nullptr);
  }
  else if (firstWord.compare("fg") == 0) {        //nullptr?
    return new ForegroundCommand(cmd_line,nullptr);
  }
  else if (firstWord.compare("quit") == 0) {     //same
    return new QuitCommand(cmd_line,nullptr);
  }
  else if (firstWord.compare("kill") == 0) {     //same
    return new KillCommand(cmd_line,nullptr);   
  }
  else if (firstWord.compare("chmod") == 0) {
    return new ChmodCommand(cmd_line);
  }
  else {
    return new ExternalCommand(cmd_line);
  }

 return nullptr;
}









void SmallShell::executeCommand(const char *cmd_line) {

  char* filePath;                              //13.2 maybe nullptr
  string cmd_s = _trim(string(cmd_line));
  bool append = containsAppendIO(cmd_line); //unintialiazed? filepath
  bool overwrite = containsOverWriteIO(cmd_line);
  int stdFD;
  if(append||overwrite) //was only with change fd
  {
    // const char* pos = strrchr(cmd_line, '>'); // Find the last occurrence of '>'
    // if (pos != nullptr && *(pos + 1) != '\0') { // Ensure '>' is not the last character
    //     size_t len = strlen(pos + 1); // Calculate the length of the substring
    //     char* extracted = (char*)malloc((len + 1) * sizeof(char)); // Allocate memory
    //     if (extracted != nullptr) {
    //         strcpy(extracted, pos + 1); // Copy the substring into the allocated memory
    //     }
    //     filePath = extracted;

    const char* pos = strrchr(cmd_line, '>'); // Find the last occurrence of '>'
    if (cmd_line != nullptr && pos != nullptr && pos != cmd_line && *(pos + 1) != '\0') {
    // Ensure '>' is not the last character and it's not the first character of cmd_line
    const char* start = pos + 1; // Start after '>'
    
    // Skip whitespace characters
    while (isspace(*start))
        start++;

    size_t len = strlen(start); // Calculate the length of the substring
    char* extracted = (char*)malloc((len + 1) * sizeof(char)); // Allocate memory
    if (extracted != nullptr) {
        strcpy(extracted, start); // Copy the substring into the allocated memory
        filePath = extracted;
    } else {
        filePath = nullptr; // Return nullptr if '>' is not found or it's the last character
    }
    stdFD = changeFD(filePath,append,overwrite);
  }
  }
  if(stdFD == -1) //this is wrong cat test_redirection1.tmp
  {
    return;
  }
  if(!append && !overwrite)
  {
    Command* cmd = CreateCommand(cmd_line); //send non trimmed
    cmd->execute();
  }
  cmd_s = IOcommandLine(cmd_s.c_str()); //also returns const char* 

  //string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  //Command* cmd = CreateCommand(cmd_s.c_str()); //send non trimmed

    if(append || overwrite)
    {
        Command* cmd = CreateCommand(cmd_s.c_str()); //sendtrimmed
        cmd->execute();
    }
  if(append || overwrite) //TODO : wrong location
  {
      resetSTD(stdFD);
  }
  // TODO: Add your implementation here
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
   //cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}

//--------------------------------------------------------------------------------------------------------------------
Command::Command(const char *cmd_line) : m_cmd_line(cmd_line) {
    // int i=0;
    // int len = strlen(cmd_line);
    // m_cmd_line = (char*) malloc((len+1)*sizeof(char));
    // for(; cmd_line[i]!='\0'; i++)
    // {
    //     m_cmd_line[i]=cmd_line[i]; // check may need changing probably not 
    // }
    // m_cmd_line[i]='\0'; //maybe problem check
}

// Command::~Command()
// {
//     free(m_cmd_line);
// }

//--------------------------------------------------------------------------------------
//ChPrompt:

ChpromptCommand::ChpromptCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

void ChpromptCommand::execute() {
    int numOfArgs=0;
    char** args = init_args(m_cmd_line,&numOfArgs); //here problem 
    if(args== nullptr)
    {
        //std::cout << "Malloc failed" << std::endl;
        perror("smash error: malloc failed");
    }
    SmallShell& tempSmash = SmallShell::getInstance();
    if(numOfArgs==1)
    {
        tempSmash.shellPrompt= "smash"; // = is enough because it's of type std::string
    }
    else
    {
        tempSmash.shellPrompt= args[1];
    }
    free_args(args,numOfArgs);
}

// show pidCommand
ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line){}

void ShowPidCommand::execute() {
    pid_t process_id ; // = DO_SYS(getpid(), getpid);
    process_id = getpid();
    if(process_id == -1)
    {
        perror("smash error: getpid failed");
        return;
    }
    std::cout << "smash pid is " << process_id << std::endl;
}

// pwd command
GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line){}

void GetCurrDirCommand::execute() {
    char path[PATH_LENGTH]; 
    //DO_SYS(getcwd(path, sizeof(path)), getcwd);
    if( getcwd(path, sizeof(path)) == NULL)
    {
        perror( "smash error: getcwd failed" ); // TODO: exit or not ? probably need to do else on rest of function 
    }
    for(int i=0; path[i]!='\0'; i++)
    {
        std::cout << path[i] ;
    }
    std::cout << std::endl;
}



//jobs list-----------------------------------------------------------------------------------------------------------



JobsList::JobsList() : currentMaxId(0){
    for(int i=0; i<100 ; ++i)
    {
        jobsListVector[i]=nullptr;
    }
}

void JobsList::addJob(Command *cmd, bool isStopped,pid_t jobPid) {
    //pid_t jobPid ; //= DO_SYS(getpid(),getpid);
    //jobPid = getpid();
    if( jobPid ==-1)
    {
        perror("smash error: getpid failed");
        return;
    }
    const char* cmdLine = cmd->m_cmd_line; //TODO: check mayebnot
    JobEntry* job = new JobEntry(currentMaxId+1,jobPid,cmdLine);  //TODO : remmember to free somewhere
    if(job==nullptr)
    {
        perror("smash error: new failed"); //maybe lo tsre5
    }
    currentMaxId++;
    //jobsListVector.push_back(job);
    for(int i=0; i< 100; ++i)
    {
        if(jobsListVector[i]==nullptr)
        {
            jobsListVector[i]= job;
            break;
        }
    }
}

void JobsList::printJobsList() {
    
    JobEntry* jobsArr[100];
    int i=0;
    int j=0;
    for(;i< 100 ; ++i)
    {
        if(jobsListVector[i]!=nullptr) //here was SEGFAULT
        {
            jobsArr[j] = jobsListVector[i]; //pointer assignment
            j++;
        }
    }
    //std::sort(jobsArr, jobsArr+j); //TODO operators for jobEntry //check for SEGFAULT should be ok
    bubbleSortWithId(jobsArr,j);
    for(int k=0; k<j; k++) //k<j ??  //operators lo hof3elo
    {
        // 11/2 here changed to job command line instead of m_command_line
        std::cout << "[" << jobsArr[k]->jobId <<"] " << jobsArr[k]->jobCommandLine << std::endl ; //TODO: maybe & is ignored in parsing check it
    }
}


void JobsList::removeFinishedJobs() {
    for(int i=0 ; i<100; ++i)
    {
        if(jobsListVector[i]!= nullptr)
        {
            int status; //this if doesnt check well or the problem is with running in background
            pid_t retWait = waitpid(jobsListVector[i]->jobPid,&status,WNOHANG);
            if(retWait ==-1)
                 std::cerr << "Waitpid failed: " << strerror(errno) << std::endl;
            if(retWait== jobsListVector[i]->jobPid)
            {
                delete jobsListVector[i];
                jobsListVector[i] = nullptr;//udpate the insert or the pushback
            }
        }
    }

    currentMaxId=0;
    for(int i=0; i< 100; ++i) {
        if (jobsListVector[i] != nullptr) {
            if (jobsListVector[i]->jobId > currentMaxId)
                currentMaxId = jobsListVector[i]->jobId;
        }
    }
}
//TODO: check destructor in jobslist

JobsCommand::JobsCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line){}

void JobsCommand::execute() {

    SmallShell& tempSmash = SmallShell::getInstance();
    tempSmash.jobsList.removeFinishedJobs();
    tempSmash.jobsList.printJobsList();
    //TODO: may need extra updates in jobsList
}

//foreGround command

ForegroundCommand::ForegroundCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line){}

bool isInteger(const std::string& str) {
    try {
        size_t pos;
        std::stoi(str, &pos);
        return pos == str.length(); // Check if entire string was consumed **
    } catch (const std::invalid_argument& e) {
        return false; // Conversion failed (not an integer)
    } catch (const std::out_of_range& e) {
        return false; // Out of range (not an integer)
    }
}


void ForegroundCommand::execute() {
    //SmallShell tempSmash = SmallShell::getInstance();

    int numOfArgs=0;
    char** args = init_args(m_cmd_line,&numOfArgs);
    if(args== nullptr)
    {
        //std::cout << "Malloc failed" << std::endl;
        perror("smash error: malloc failed");
    }
    SmallShell& tempSmash = SmallShell::getInstance();
    if(numOfArgs==1)
    {
        int maxId;
        tempSmash.jobsList.removeFinishedJobs();
        JobsList::JobEntry* job = tempSmash.jobsList.getLastJob(&maxId);
        //tempSmash.jobsList.removeFinishedJobs();
        //job = tempSmash.jobsList.getLastJob(&maxId); //added  now
        if(job== nullptr && maxId==0) //TODO : checking if the list is empty
        {
            std::cerr << "smash error: fg: jobs list is empty" << std::endl;
        }
        else
        {
            std::cout << job->jobCommandLine <<" "<< job->jobPid << std::endl;
            tempSmash.currentFgProcess = job->jobPid;
            int status;
            waitpid(job->jobPid, &status,0); // 11/2 0 for optional parameter //DO_SYS???
            tempSmash.jobsList.removeJobById(job->jobId); //TODO : UPDATING MAX IN remove
            tempSmash.jobsList.removeFinishedJobs(); //recalculate max 
            tempSmash.currentFgProcess = 0 ;
        }
    }
    else if(numOfArgs==2)
    {
        std::string firstArg(args[1]);
        if(!isInteger(firstArg))
        {
            std::cerr<< "smash error: fg: invalid arguments"<<std::endl;
        }
        else {
            JobsList::JobEntry *job = tempSmash.jobsList.getJobById(std::stoi(args[1]));
            if (job == nullptr) //job with specified id doesn't exist
            {
                std::cerr << "smash error: fg: job-id " << std::stoi(args[1]) << " does not exist" <<std::endl;       //adding " " 13.2 9:47
            } else {
                std::cout << job->jobCommandLine << " " << job->jobPid << std::endl;
                tempSmash.currentFgProcess = job->jobPid;
                int status;
                waitpid(job->jobPid, &status,0); // 11/2 0 for optional parameter
                tempSmash.jobsList.removeJobById(std::stoi(args[1])); //TODO : UPDATING MAX IN remove
                tempSmash.jobsList.removeFinishedJobs();
                tempSmash.currentFgProcess=0;
            }
        }


    }
    else
    {
        std::string firstArg(args[1]);
        JobsList::JobEntry *job = nullptr;
        if(isInteger(firstArg)) 
        {
            job = tempSmash.jobsList.getJobById(std::stoi(args[1]));
        }
        if(!isInteger(firstArg))
        {
            std::cerr<< "smash error: fg: invalid arguments"<<std::endl;
        }
        //job = tempSmash.jobsList.getJobById(std::stoi(args[1]));
        else if(job == nullptr) //job with specified id doesn't exist
        {
            std::cerr << "smash error: fg: job-id " << std::stoi(args[1]) << " does not exist" <<std::endl;       //adding " " 13.2 9:47
        } 
        else
        {
            std::cerr << "smash error: fg: invalid arguments"<< std::endl;
        }
    }
    free_args(args,numOfArgs);
}



//KillCommand
void bubbleSort(JobsList::JobEntry** arr,int size) {
    int n = size;
    bool swapped;

    for (int i = 0; i < n - 1; i++) {
        swapped = false;

        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j]->jobPid > arr[j + 1]->jobPid) {                //JobEntry* //swap check
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        // If no two elements were swapped by inner loop, the array is already sorted
        if (!swapped) {
            break;
        }
    }
}











//KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line){}

void QuitCommand::execute() {
    int numOfArgs=0;
    char** args = init_args(m_cmd_line,&numOfArgs);
    if(args== nullptr)
    {
        //std::cout << "Malloc failed" << std::endl;
        perror("smash error: malloc failed");
    }
    SmallShell& tempSmash = SmallShell::getInstance();

    if(numOfArgs==1 || ((numOfArgs>1) && (strcmp(args[1], "kill")!=0)) )//no parameters were sent
    {
        // for(int k=0; k<100 ; ++k) //check alone 
        // {
        //     if(tempSmash.jobsList.jobsListVector[k]!= nullptr)
        //         delete (tempSmash.jobsList.jobsListVector[k]); //check for more memory issues
        //         //delete calls the class destructor so its not freed, coordinate with 
        //         // case that kill wasn't specified
        // }
       // DO_SYS(exit(0), exit); 
       //exit calls destructors of global objects like jobsListVector TODO: check rest *
       free_args(args,numOfArgs);
       exit(0); ///only global objects destroyed automatically
    }
    else if(strcmp(args[1],"kill")==0) //check : if more arguments after kill what should we do 
    { //temporarily without sorting
        tempSmash.jobsList.removeFinishedJobs();
        //TODO: do we need to sort wrt pid before printing
        //sorting
        JobsList::JobEntry* jobsArr[100];
        int i=0;
        int j=0;
        for(;i< 100 ; ++i)
        {
            if(tempSmash.jobsList.jobsListVector[i]!= nullptr)
            {
                jobsArr[j] = tempSmash.jobsList.jobsListVector[i]; //pointer assignment
                j++;
            }
        }
        bubbleSort(jobsArr,j); //here j isn't right
        //tempSmash.jobsList.removeFinishedJobs();
        int jobsNum=0;
        for(int k=0; k<100; ++k)
        {
            // int d = tempSmash.jobsList.jobsListVector[k]->jobId;
            // JobsList::JobEntry* ptr=nullptr;
            // ptr = tempSmash.jobsList.jobsListVector[k]; //here acess is wrong
            // pid_t s = ptr->jobPid;
            if(tempSmash.jobsList.jobsListVector[k]!= nullptr)
            {
                jobsNum++;
            }
        }
        std::cout << "smash: sending SIGKILL signal to "<< jobsNum<<" jobs:"<< std::endl;
        // for(int k=0; k<jobsNum ; ++k)
        // {
            // at this point they are still in the list ? probably not but still  in jobsArr
            //better to witch it to the actual vector
            //std::cout << jobsArr[k]->jobPid<< ": "<< jobsArr[k]->jobCommandLine << std::endl;
            for(int i=0; i<j ; ++i)
            {
                //if(jobsArr[i]!=nullptr){
                if(kill(jobsArr[i]->jobPid,9) == -1)
                {
                    perror("smash error: kill failed");
                }
                else{
                    std::cout << jobsArr[i]->jobPid<< ": "<< jobsArr[i]->jobCommandLine << std::endl;
                }
                //DO_SYS(kill((tempSmash.jobsList.jobsListVector[i]->jobPid), 9), kill) ;
                //}
            }
            //DO_SYS(kill((jobsArr[k]->jobPid), 9), kill) ;
        //}
        // for(int k=0; k<j ; ++k)
        // {
        //     delete jobsArr[k]; //check for more memory issues
        // }
        //TODO: no need to delete because destructor is called in exit(0);
        //DO_SYS(exit(0),exit);
        free_args(args,numOfArgs);
        exit(0); //only global objects destroyed automatically
    }

    free_args(args,numOfArgs); //TODO: more on this
}

KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) : BuiltInCommand(cmd_line){}

void KillCommand::execute() {
    int numOfArgs=0;
    char** args = init_args(m_cmd_line,&numOfArgs);
    if(args== nullptr)
    {
        //std::cout << "Malloc failed" << std::endl;
        perror("smash error: malloc failed");
    }
    SmallShell& tempSmash = SmallShell::getInstance();
    if((numOfArgs >= 3) && (isInteger(args[2])) && (tempSmash.jobsList.getJobById(std::stoi(args[2]))== nullptr))
    {
        std::cerr << "smash error: kill: job-id "<< std::stoi(args[2]) <<" does not exist" << std::endl; //TODO : do we need stoi
    }
    else if(numOfArgs!=3 || (!isInteger(args[1]))  || (!isInteger(args[2])) || (std::stoi(args[1])>=0) )
    {
       std::cerr<< "smash error: kill: invalid arguments" <<std::endl;
    }
    else
    {
        JobsList::JobEntry * job = tempSmash.jobsList.getJobById(std::stoi(args[2]));
        pid_t jobPid = job->jobPid;
        int SIG = std::stoi(args[1]);
        SIG = SIG * (-1);
        //DO_SYS(kill(jobPid,SIG), kill);
        if(kill(jobPid,SIG) == -1)
        {
            perror("smash error: kill failed");
            free_args(args,numOfArgs);
            return;
        }
        std::cout << "signal number " << SIG <<" was sent to pid "<< jobPid << std::endl;
        if(SIG==9) //TODO : before or after kill , shouldnt be important
        {
            tempSmash.jobsList.removeJobById(std::stoi(args[2]));
            tempSmash.jobsList.removeFinishedJobs();
        }
    }
    free_args(args,numOfArgs);
}






//cd command

ChangeDirCommand::ChangeDirCommand(const char *cmd_line, char **plastPwd) : BuiltInCommand(cmd_line), notUsed(plastPwd)  {}

void ChangeDirCommand::execute() {

    int numOfArgs=0;
    char** args = init_args(m_cmd_line,&numOfArgs);
    if(args== nullptr)
    {
        //std::cout << "Malloc failed" << std::endl;
        perror("smash error: malloc failed");
    }
    SmallShell &tempSmash = SmallShell::getInstance();
    if(numOfArgs==1)  //TODO: in case there is no parameters for cd WHAT TO DO ??
    {
        //tempSmash.shellPrompt= "smash"; // = is enough because it's of type std::string

    }
    else if( numOfArgs >2)
    {
        std::cerr<< "smash error: cd: too many arguments"<< std::endl;
    }
    else if( numOfArgs==2)
    {
        if(strcmp(args[1],"-")==0) //check
        {
            if( tempSmash.lastWorkingDirectory== "")
            {
                std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
            }
            else // check here if same problem not necessary to change it
            {
                std::string tempDirectory = tempSmash.lastWorkingDirectory;
                //DO_SYS(chdir(tempDirectory.c_str()), chdir); //same problem as below fix it 
                int retVal = chdir(tempDirectory.c_str());
                if(retVal==0){
                tempSmash.lastWorkingDirectory= tempSmash.currentWorkingDirectory; //check
                tempSmash.currentWorkingDirectory = tempDirectory;
                }
                else if(retVal==-1)
                {
                    perror("smash error: chdir failed");
                }
            }
        }
        else // cd .. routed here
        {
            //DO_SYS(chdir(args[1]), chdir); //problem : we update even if the syscall fails
            char path[PATH_LENGTH]; 
            int retVal = chdir(args[1]);
            if(retVal==0){
            tempSmash.lastWorkingDirectory= tempSmash.currentWorkingDirectory; //check
            //tempSmash.currentWorkingDirectory = args[1]; //if i send .. they will be saved here
            if( getcwd(path, sizeof(path)) == NULL)
            {
                perror( "smash error: getcwd failed" ); // TODO: exit or not ? probably need to do else on rest of function 
            }
            tempSmash.currentWorkingDirectory = path ; //no problem with memory
            }
            else if(retVal==-1)
            {
                perror("smash error: chdir failed");
            } 
        }
    }
    free_args(args,numOfArgs);
}

//---------------------------------------------------------------------------------------------------
// external commands

ExternalCommand::ExternalCommand(const char *cmd_line) :Command(cmd_line){}


bool containsWildcard(const char* str) {
    while (*str) {
        if (*str == '?' || *str == '*') {
            return true;
        }
        ++str;
    }
    return false;
}

bool isPath(const char* str) {
    while (*str) {
        if (*str == '/') {
            return true;
        }
        ++str;
    }
    return false;
}


//somewhere we need to check if its external or built in page 10 in hw note 1
void ExternalCommand::execute() {
    int numOfArgs=0; //num of args is calculated without null terminator
    char* cmdLineStrippedFromBg= (char*) (malloc((strlen(m_cmd_line)+1)*sizeof(char))); //TODO: remember to free
    strcpy(cmdLineStrippedFromBg,m_cmd_line);
    _removeBackgroundSign(cmdLineStrippedFromBg);
    char** args = init_args(cmdLineStrippedFromBg,&numOfArgs); //strips the &??
    if(args== nullptr)
    {
        //std::cout << "Malloc failed" << std::endl;
        perror("smash error: malloc failed");
    }
     char* passArgs[numOfArgs+1]; 
    int i=0;
    for(; i <numOfArgs; ++i) //TODO: IMPORTANT NEEDS CHECKING
    {   
        // if(strcmp(args[i+1], "&")!=0)// TODO: this check might be problem
        // {
            passArgs[i]=args[i]; //problem with dynamically allocated and execv check ?
        // }
        // else
        // {
        //     passArgs[i]=NULL;
        //     break;
        // }
    }
    passArgs[i]=NULL;  
    if(containsWildcard(m_cmd_line)) //complex
    {
        char* passArgsBash[numOfArgs+3]; //changed here 15/2 was+1 and added lines down

        //------------------------------------------------------------------------------------------
        // Length of the string, including the null terminator
        size_t length = strlen("/bin/bash") + 1;

        // Allocate memory for the string
        char *progName = (char *)malloc(length * sizeof(char));

        // Check if memory allocation was successful
        if (progName == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return; // Exit with error code
        }
        // Copy the string "/bin/bash" into the allocated memory
        strcpy(progName, "/bin/bash");




        //---------------------------------------------------------------------------------------------
       // char* progName= "/bin/bash";            //here added 11/2
        passArgsBash[0]=progName;
        
        //-------------------------------------------------------------------------------------
            // Length of the string, including the null terminator
        size_t secondLength = strlen("-c") + 1;

        // Allocate memory for the string
        char *specific = (char *)malloc(secondLength * sizeof(char));

        // Check if memory allocation was successful
        if (specific == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            return; // Exit with error code
        }

        // Copy the string "/bin/bash" into the allocated memory
        strcpy(specific, "-c"); 
    //-------------------------------------------------------------------------
        //char* specific= "-c";            //here added 11/2
        passArgsBash[1]=specific;
        for(int i=0; i<numOfArgs ; ++i)
        {
            passArgsBash[i+2]=passArgs[i];     //chnaged 15/2 was i+1
        }
        if(_isBackgroundComamnd(m_cmd_line))
        {
            SmallShell& tempSmash = SmallShell::getInstance();
            ShowPidCommand commandObj = ShowPidCommand(m_cmd_line);
            //tempSmash.jobsList.addJob(&commandObj); //TODO : add the line or with bash -c line ?
            pid_t pid = fork(); //FDT copied from parent
            if(pid==0)
            {
                DO_SYS(setpgrp(),setpgrp);
                if(execv("/bin/bash",passArgsBash) == -1)
                {
                    perror("smash error: execv failed");
                    exit(-10);
                }
            }
            else if(pid>0)
            {
                //change the commandobj

                tempSmash.jobsList.addJob(&commandObj,false,pid);
            }
        }
        else // complex in foreground
        {
            pid_t pid = fork();
            if(pid==0)
            {
                DO_SYS(setpgrp(),setpgrp);
                if(execv("/bin/bash",passArgsBash) == -1)
                {
                    perror("smash error: execv failed");
                    exit(-10);
                }
            }
            else
            {
                SmallShell& tempSmash = SmallShell::getInstance();    // maybe we need pid tb3 abn al bash 
                tempSmash.currentFgProcess = pid;
                int status;
                waitpid(pid,&status,0); // 11/2 also 0 for optional parameter
                tempSmash.currentFgProcess = 0;                     // what happened if kebalno sigstp
            }

        }
    }
    else //simple external command
    {
        if(isPath(args[0])) //need to use execv
        {
            if(_isBackgroundComamnd(m_cmd_line))
            {
                SmallShell& tempSmash = SmallShell::getInstance();
                //tempSmash.jobsList.addJob(cmd_line);
                //Command commandObj = Command(m_cmd_line);
                ShowPidCommand commandObj = ShowPidCommand(m_cmd_line);
                //tempSmash.jobsList.addJob(&commandObj); //TODO : add the line or with bash -c line ?
                pid_t pid = fork();
                if(pid==0)
                {
                    DO_SYS(setpgrp(),setpgrp);
                    if(execv(args[0],passArgs) == -1)
                    {
                        perror("smash error: execv failed");
                        exit(-10);
                    } 
                }
                else if(pid>0)
                {
                    tempSmash.jobsList.addJob(&commandObj,false,pid);
                }
            }
            else // simple external with path not in background
            {

                pid_t pid = fork();
                if(pid==0)
                {
                    DO_SYS(setpgrp(),setpgrp);
                    if(execv(args[0],passArgs) == -1)
                    {
                        perror("smash error: execv failed");
                        exit(-10);
                    }
                }
                else
                {
                    SmallShell& tempSmash = SmallShell::getInstance();
                    tempSmash.currentFgProcess = pid;
                    int status;
                    waitpid(pid,&status,0); // 11/2 0 for optional paramter
                    tempSmash.currentFgProcess = 0;                     // what happened if kebalno sigstp
                }
            }

        }
        else // simple external wihtout path
        {
            if(_isBackgroundComamnd(m_cmd_line))
            {
                SmallShell& tempSmash = SmallShell::getInstance();
                //tempSmash.jobsList.addJob(cmd_line);
                ShowPidCommand commandObj = ShowPidCommand(m_cmd_line);
                //tempSmash.jobsList.addJob(&commandObj); //TODO : add the line or with bash -c line ?
                pid_t pid = fork();
                if(pid==0)
                {
                    DO_SYS(setpgrp(),setpgrp);
                    if(execvp(args[0],passArgs) == -1)
                    {
                        perror("smash error: execvp failed");
                        exit(-10);
                    }
                }
                else if(pid>0)
                {
                    tempSmash.jobsList.addJob(&commandObj,false,pid);
                }
            }
            else // simple external without path not in background
            {

                pid_t pid = fork();
                if(pid==0)
                {
                    DO_SYS(setpgrp(),setpgrp);
                    if(execvp(args[0],passArgs) == -1)
                    {
                        perror("smash error: execvp failed");
                        exit(-10);
                    }
                    
                }
                else
                {
                    SmallShell& tempSmash = SmallShell::getInstance();
                    tempSmash.currentFgProcess = pid;
                    int status;
                    waitpid(pid,&status,0); // 11/2 0 for optional parameter
                    tempSmash.currentFgProcess = 0;                     // what happened if kebalno sigstp
                }
            }


        }


    }


    free_args(args,numOfArgs);

}


//chmod command
ChmodCommand::ChmodCommand(const char *cmd_line) : BuiltInCommand(cmd_line){}

void ChmodCommand::execute() {
    int numOfArgs=0;
    char** args = init_args(m_cmd_line,&numOfArgs);
    if(args== nullptr)
    {
        perror("smash error: malloc failed");
        free_args(args,numOfArgs);
        return;
    }
    //TODO: input checks should be ok might need to check 
    //SmallShell& tempSmash = SmallShell::getInstance();
    if (numOfArgs>3 || numOfArgs<3 || !(isInteger(args[1])) ) //TODO: rest of input checks
    {
        std::cerr << "smash error: chmod: invalid arguments" << std::endl;

    }
    else
    {
        mode_t mode;
        const char* path = args[2]; //TODO: need to check if the file exists or we can access it, asked in piazza
        const char* flags = args[1];
        mode = strtol(flags, NULL, 8); //pass permissions to chmod syscall
        DO_SYS(chmod(path,mode), chmod);

    }
    free_args(args,numOfArgs);
}

//may need to disable
JobsList::~JobsList() //called twice in quit command 
{
    for(int i=0; i<100 ; ++i)
    {
        if(jobsListVector[i]!=nullptr)
        {
            delete jobsListVector[i];
        }
    }
}
