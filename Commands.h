#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>



#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
// TODO: Add your data members
//protected:

 public:

  const char* m_cmd_line;
  Command(const char* cmd_line);
  virtual ~Command()= default; //default added now 11/2
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand() = default;
};

class ExternalCommand : public Command {
 public:
  ExternalCommand(const char* cmd_line);
  virtual ~ExternalCommand() = default;
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() =default;
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() =default;
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members 
public:
  char** notUsed;      // not used parameter dummy
  ChangeDirCommand(const char* cmd_line, char** plastPwd);
  virtual ~ChangeDirCommand() =default;
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() = default;
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() = default;
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members 
public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() = default;
  void execute() override;
};




class JobsList { //TODO: maybe need to make it member of smash class
 public:
  class JobEntry {
   // TODO: Add your data members
  public:
      int jobId;
      pid_t jobPid;
      std::string jobCommandLine; //TODO: use c._str
      JobEntry(): jobId(0){}  // TODO: check?
      JobEntry(int jobId,pid_t jobPid, std::string jobCommandLine) : jobId(jobId), jobPid(jobPid), jobCommandLine(jobCommandLine)
      {}//do
      ~JobEntry() = default;
      //operators for std::sort
      // bool operator<(const JobEntry& other) 
      // {
      //     if(jobId < other.jobId)
      //         return true;
      //     return false;
      // }

      // bool operator>(const JobEntry& other) 
      // {
      //     return !(*(this)<other);
      // }

      // bool operator==(const JobEntry& other) 
      // {
      //     if(jobId == other.jobId)
      //         return true;
      //     return false;
      // }

      // bool operator!=(const JobEntry& other) 
      // {
      //     if(jobId != other.jobId)
      //         return true;
      //     return false;
      // }
  };
 // TODO: Add your data members
 public:
  JobEntry* jobsListVector[100]; // default c'tor of vector will be called in the c'tor
  int currentMaxId; // TODO: initialzed to 0, in each new insertion we do jobID=currentmax+1
  JobsList();
  ~JobsList(); // TODO: surely needs to be implemented
  void addJob(Command* cmd, bool isStopped = false,pid_t jobPid = -3); // can just pass the cmd and parse it inside using the given functions
  //TODO: fork and exec for the job here ?  the list is only for external commands that run in bg
  void printJobsList(); // when emptying a space in vector could mark it with special mark to avoid it
  void killAllJobs(); //TODO :together with kill command
  void removeFinishedJobs(); //TODO: here we have to update the maxid member and maybe other things
  JobEntry * getJobById(int jobId)
  {
      for(int i=0 ; i<100; ++i)
      {
        if(jobsListVector[i] != nullptr)
        {
          if(jobsListVector[i]->jobId == jobId)
            return jobsListVector[i];
        }
      }
      return nullptr;
  }
  void removeJobById(int jobId)
  {
      for(int i=0; i<100; ++i)
      {
        if(jobsListVector[i]!=nullptr){
          if(jobsListVector[i]->jobId == jobId) //segfault
          {
              delete jobsListVector[i];
              jobsListVector[i]=nullptr;
          }
        }
      }
      currentMaxId=0;
      for(int l=0; l< 100; ++l) {
          if (jobsListVector[l] != nullptr) {
              if (jobsListVector[l]->jobId > currentMaxId)
                  currentMaxId = jobsListVector[l]->jobId;
          }
      }
  }

  JobEntry * getLastJob(int* lastJobId) //check here tloy b currentmaxid etha s7
  {
      for(int i=0; i<100; ++i)
      {
        if(jobsListVector[i] != nullptr)                          //add new 13.2 8:54
        {
          if(jobsListVector[i]->jobId==currentMaxId) {            //add newcheck 13.2
              *lastJobId= jobsListVector[i]->jobId;
              return jobsListVector[i];
          }
        }
      }
      *lastJobId=0;
      return nullptr;
  }
  JobEntry *getLastStoppedJob(int *jobId); //TODO : later
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs);
  virtual ~JobsCommand() =default; //TODO: here may need to make it custom
  void execute() override; // removes finsihed jobs, sorts the list, prints it
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line, JobsList* jobs); //may need to add memebr jobs
  virtual ~KillCommand() = default;
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand()= default;
  void execute() override;
};

class ChmodCommand : public BuiltInCommand {
 public:
  ChmodCommand(const char* cmd_line);
  virtual ~ChmodCommand() = default;
  void execute() override;
};


class SmallShell {
 private:
  // TODO: Add your data members
  SmallShell();
 public:

  static pid_t currentFgProcess; //TODO :initialize to 0 in constructor
  JobsList jobsList;   //TODO : update constructor to initialize the jobs list
  static std::string  lastWorkingDirectory;    //TODO: update in constructor check if need to do anything
  //may  need to make it malloc check later
  static std::string  currentWorkingDirectory; //TODO: update in constructor
  static std::string shellPrompt;
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell() = default; //not default
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};
//----------------------------------------------------------------------------------------------------
// our classes :


class ChpromptCommand : public BuiltInCommand {
// TODO: Add your data members public:
    public: 
    ChpromptCommand(const char* cmd_line);
    virtual ~ChpromptCommand() {} //check
    void execute() override;
};



#endif //SMASH_COMMAND_H_
