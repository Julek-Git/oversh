#include <iostream>
#include <map>
#include <vector>


#include <cstdlib>
#include <cstring>

namespace sys {
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>
}

#include "builtin.hpp"
#include "cwd.hpp"

std::vector<std::string> command_history;

void run(std::string file, std::string args);


int main() {
  std::map<std::string, void (*)(std::string)> builtin_commands;

  builtin_commands["cd"] = builtin::cd;
  builtin_commands["history"] = builtin::history;


  while (1) {
    char hostname_buf[1024];
    sys::gethostname(hostname_buf, 1024);
    std::cout << utils::getcwd() << std::endl << sys::getlogin() << "@" << hostname_buf << "$ " << std::flush;

    std::string command = "";
    std::string prog_args = "";

    std::getline(std::cin, command);
    size_t space_pos = command.find(' ');
    if (space_pos != std::string::npos) {
      prog_args = command.substr(space_pos + 1);
      command = command.substr(0, space_pos);
    }

    command_history.push_back(command + " " + prog_args);

    if (command == "exit") {
      break;
    }

    if (builtin_commands.count(command)) {
      builtin_commands[command](prog_args);
    } else {
      run(command, prog_args);
    }
  }

  return 0;
}

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