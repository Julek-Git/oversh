#ifndef BUILTIN_HPP
#define BUILTIN_HPP

#include <string>

namespace builtin {
  void cd(std::string);
  void history(std::string);
  void alias(std::string);
}

#endif // BUILTIN_HPP