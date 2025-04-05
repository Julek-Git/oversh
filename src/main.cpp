#include <iostream>
#include <map>
#include <vector>

#include <cstdlib>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>

std::string cwd = "";
std::vector<std::string> command_history;

void run(std::string file, std::string args);

namespace builtin {
  void cd(std::string);
  void history(std::string);
}


int main() {
  std::map<std::string, void (*)(std::string)> builtin_commands;

  builtin_commands["cd"] = builtin::cd;
  builtin_commands["history"] = builtin::history;

  char cwd_buf[PATH_MAX];
  getcwd(cwd_buf, sizeof(cwd_buf));
  cwd = cwd_buf;


  while (1) {
    std::cout << cwd << "$ " << std::flush;

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
  pid_t pid = fork();

  if (pid == 0) {
    char *argv[] = { strdup(file.c_str()), strdup(args.c_str()), NULL };
    
    execvp(file.c_str(), argv);

    std::cerr << "exec failed: " << strerror(errno) << std::endl;
    exit(1);
  } else if (pid > 0) {
    wait(NULL);
  } else {
    std::cerr << "fork failed: " << strerror(errno) << std::endl;
  }
}

void builtin::cd(std::string new_path) {
  if (new_path == "") {
    std::cerr << "cd: missing argument\n";
    return;
  }

  if (new_path[0] == '/') {
    if (chdir(new_path.c_str()) == -1) {
      std::cerr << "cd: " << strerror(errno) << "\n";
    } else {
      cwd = new_path;
    }
  } else {
    std::string full_path = cwd + "/" + new_path;
    if (chdir(full_path.c_str()) == -1) {
      std::cerr << "cd: " << strerror(errno) << "\n";
    } else {
      char cwd_buf[PATH_MAX];
      getcwd(cwd_buf, sizeof(cwd_buf));
      cwd = cwd_buf;
    }
  }
}

void builtin::history(std::string) {
  for (const std::string &cmd : command_history) {
    std::cout << cmd << "\n";
  }
}
