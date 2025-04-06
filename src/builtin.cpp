#include "builtin.hpp"

#include <iostream>
#include <string>
#include <vector>


#include "cwd.hpp"


void builtin::cd(std::string new_path) {
  if (new_path == "") {
    std::cerr << "cd: missing argument\n";
    return;
  }

  utils::setcwd(new_path);
}


extern std::vector<std::string> command_history;
void builtin::history(std::string) {
  for (const std::string &cmd : command_history) {
    std::cout << cmd << "\n";
  }
}
