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
#include "run_process.h"

std::vector<std::string> command_history;

void run(std::string file, std::string args);


int main() {
  std::map<std::string, void (*)(std::string)> builtin_commands;

  builtin_commands["cd"] = builtin::cd;
  builtin_commands["history"] = builtin::history;


  while (1) {
    char hostname_buf[1024];
    sys::gethostname(hostname_buf, 1024);
    std::cout << sys::getlogin() << "@" << hostname_buf << " " << utils::getcwd() << "$ " << std::flush;

    std::string command = "";
    std::string prog_args = "";

    std::getline(std::cin, command);

    if (command.empty()) continue;

    command_history.push_back(command);

    if (command == "exit") break;

    size_t space_pos = command.find(' ');
    std::string base_cmd = command.substr(0, space_pos);

    if (builtin_commands.count(base_cmd)) {
      std::string args = space_pos != std::string::npos ? command.substr(space_pos + 1) : "";
      builtin_commands[base_cmd](args);
    } else {
      run_pipeline(command);
    }

  }

  return 0;
}