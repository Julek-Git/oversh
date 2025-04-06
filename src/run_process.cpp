#include <iostream>
#include <string>
#include <vector>
#include <sstream>


#include <cstdlib>
#include <cstring>

namespace sys {
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>
}

/* run - old unused command */
void run(std::string file, std::string args) {
  pid_t pid = sys::fork();

  if (pid == 0) {
    sys::execlp(file.c_str(), args.c_str(), NULL);

    std::cerr << "exec failed: " << strerror(errno) << std::endl;
    exit(1);
  } else if (pid > 0) {
    sys::wait(NULL);
  } else {
    std::cerr << "fork failed: " << strerror(errno) << std::endl;
  }
}


void run_pipeline(std::string input) {
  std::vector<std::string> stages;
  size_t pos = 0;
  while ((pos = input.find('|')) != std::string::npos) {
    stages.push_back(input.substr(0, pos));
    input.erase(0, pos + 1);
  }
  stages.push_back(input);

  int prev_pipe_fd[2] = {-1, -1};

  for (size_t i = 0; i < stages.size(); ++i) {
    std::string& stage = stages[i];

    std::vector<char*> args;
    std::istringstream iss(stage);
    std::string token;
    while (iss >> token) {
      args.push_back(strdup(token.c_str()));
    }
    args.push_back(NULL);

    int pipe_fd[2];
    if (i != stages.size() - 1) {
      if (sys::pipe(pipe_fd) == -1) {
        std::cerr << "pipe failed\n";
        return;
      }
    }

    pid_t pid = sys::fork();
    if (pid == 0) {
      if (i != 0) {
        sys::dup2(prev_pipe_fd[0], STDIN_FILENO);
        sys::close(prev_pipe_fd[0]);
        sys::close(prev_pipe_fd[1]);
      }

      if (i != stages.size() - 1) {
        sys::dup2(pipe_fd[1], STDOUT_FILENO);
        sys::close(pipe_fd[0]);
        sys::close(pipe_fd[1]);
      }

      sys::execvp(args[0], args.data());
      std::cerr << "exec failed: " << strerror(errno) << std::endl;
      exit(1);
    } else if (pid < 0) {
      std::cerr << "fork failed: " << strerror(errno) << std::endl;
      return;
    }

    if (i != 0) {
      sys::close(prev_pipe_fd[0]);
      sys::close(prev_pipe_fd[1]);
    }
    if (i != stages.size() - 1) {
      prev_pipe_fd[0] = pipe_fd[0];
      prev_pipe_fd[1] = pipe_fd[1];
      sys::close(pipe_fd[1]);
    }
  }

  for (size_t i = 0; i < stages.size(); ++i) {
    sys::wait(NULL);
  }
}
