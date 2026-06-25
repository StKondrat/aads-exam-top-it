#include <cstddef>
#include <string>

namespace kondrat
{
  struct Person
  {
    size_t id;
    std::string info;
  };

  struct PersonStorage
  {
    Person * data;
    size_t size;
    size_t capacity;
  };

  struct ProgramArgs
  {
    const char * inputName;
    const char * outputName;
  };

  bool startsWith(const char * value, const char * prefix);
  bool parseArgs(int argc, char ** argv, ProgramArgs & args);
}

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

int main(int argc, char ** argv)
{}
