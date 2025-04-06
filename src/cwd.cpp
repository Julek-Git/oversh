#include "cwd.hpp"

#include <iostream>
#include <string>

#include <cstring>

namespace sys {
#include <unistd.h>
#include <linux/limits.h>
}


std::string utils::getcwd() {
  char cwd_buf[PATH_MAX];
  sys::getcwd(cwd_buf, sizeof(cwd_buf));
  return (std::string) cwd_buf;
}

void utils::setcwd(std::string cwd) {
  int status = sys::chdir(cwd.c_str());
  if (status == -1) {
    std::cerr << "cd: " << strerror(errno) << "\n";
  }
}