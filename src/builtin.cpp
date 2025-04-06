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

extern std::vector<std::pair<std::string, std::string>> command_aliases;
void builtin::alias(std::string alias) {
  size_t pos = alias.find('=');
  if (pos == std::string::npos) {
    std::cerr << "alias: invalid alias format\n";
    return;
  }
  std::string alias_name = alias.substr(0, pos);
  std::string alias_value = alias.substr(pos + 1);
  command_aliases.push_back({alias_name, alias_value});
}