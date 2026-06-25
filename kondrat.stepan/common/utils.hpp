#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <string>

namespace kondrat
{
  bool startsWith(const char * value, const char * prefix);
  bool isSpace(char value);
  bool isDigit(char value);
  bool readSizeT(const std::string & line, size_t & pos, size_t & value);
  size_t nextCapacity(size_t capacity);
}

#endif
