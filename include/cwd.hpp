#ifndef CWD_HPP
#define CWD_HPP

#include <string>

namespace utils {
  std::string getcwd();
  void setcwd(std::string cwd);
}

#endif // CWD_HPP