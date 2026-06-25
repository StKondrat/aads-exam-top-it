#include <cstddef>
#include <string>

namespace kondrat
{
  struct Person
  {
    size_t id;
    std::string info;
    bool described;
  };

  struct Meeting
  {
    size_t first;
    size_t second;
    size_t time;
  };

  struct PersonStorage
  {
    Person * data;
    size_t size;
    size_t capacity;
  };

  struct MeetingStorage
  {
    Meeting * data;
    size_t size;
    size_t capacity;
  };

  struct ProgramArgs
  {
    const char * input;
    const char * data;
  };
}

int main()
{}
