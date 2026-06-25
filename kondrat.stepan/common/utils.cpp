#include "utils.hpp"
#include <limits>
#include <stdexcept>

bool kondrat::startsWith(const char * value, const char * prefix)
{
  while (*prefix != '\0')
  {
    if (*value != *prefix)
    {
      return false;
    }
    ++value;
    ++prefix;
  }
  return true;
}

bool kondrat::isSpace(char value)
{
  return value == ' ' || value == '\t' || value == '\v' || value == '\f' || value == '\r';
}

bool kondrat::isDigit(char value)
{
  return value >= '0' && value <= '9';
}

bool kondrat::readSizeT(const std::string & line, size_t & pos, size_t & value)
{
  if (pos == line.size() || !isDigit(line[pos]))
  {
    return false;
  }

  value = 0;
  while (pos < line.size() && isDigit(line[pos]))
  {
    const size_t digit = static_cast< size_t >(line[pos] - '0');
    if (value > (std::numeric_limits< size_t >::max() - digit) / 10)
    {
      return false;
    }
    value = value * 10 + digit;
    ++pos;
  }
  return true;
}

size_t kondrat::nextCapacity(size_t capacity)
{
  if (capacity == 0)
  {
    return 8;
  }
  if (capacity > std::numeric_limits< size_t >::max() / 2)
  {
    throw std::overflow_error("storage capacity overflow");
  }
  return capacity * 2;
}
