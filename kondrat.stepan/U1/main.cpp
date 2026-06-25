#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
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
    const char * input;
    const char * output;
  };

  bool startsWith(const char * value, const char * prefix);
  bool parseArgs(int argc, char ** argv, ProgramArgs & args);
  void initStorage(PersonStorage & storage);
  void destroyStorage(PersonStorage & storage);
  bool isSpace(char value);
  bool isDigit(char value);
  bool parsePerson(const std::string & line, Person & person);
  bool containsId(const PersonStorage & storage, size_t id);
  size_t nextCapacity(size_t capacity);
  void reserve(PersonStorage & storage, size_t capacity);
  void pushBack(PersonStorage & storage, const Person & person);
  void readPersons(std::istream & input, PersonStorage & storage, size_t & ignored);
  void printPersons(std::ostream & output, const PersonStorage & storage);
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

bool kondrat::parseArgs(int argc, char ** argv, ProgramArgs & args)
{
  args.input = nullptr;
  args.output = nullptr;
  if (argc > 3)
  {
    return false;
  }

  for (int i = 1; i < argc; ++i)
  {
    if (startsWith(argv[i], "in:"))
    {
      if (args.input != nullptr || argv[i][3] == '\0')
      {
        return false;
      }
      args.input = argv[i] + 3;
    }
    else if (startsWith(argv[i], "out:"))
    {
      if (args.output != nullptr || argv[i][4] == '\0')
      {
        return false;
      }
      args.output = argv[i] + 4;
    }
    else
    {
      return false;
    }
  }

  return true;
}

void kondrat::initStorage(PersonStorage & storage)
{
  storage.data = nullptr;
  storage.size = 0;
  storage.capacity = 0;
}

void kondrat::destroyStorage(PersonStorage & storage)
{
  delete[] storage.data;
  storage.data = nullptr;
  storage.size = 0;
  storage.capacity = 0;
}

bool kondrat::isSpace(char value)
{
  return value == ' ';
}

bool kondrat::isDigit(char value)
{
  return value >= '0' && value <= '9';
}

bool kondrat::parsePerson(const std::string & line, Person & person)
{
  size_t first = 0;
  while (first < line.size() && isSpace(line[first]))
  {
    ++first;
  }
  if (first == line.size() || !isDigit(line[first]))
  {
    return false;
  }

  size_t id = 0;
  while (first < line.size() && isDigit(line[first]))
  {
    const size_t digit = static_cast< size_t >(line[first] - '0');
    if (id > (std::numeric_limits< size_t >::max() - digit) / 10)
    {
      return false;
    }
    id = id * 10 + digit;
    ++first;
  }

  size_t afterId = first;
  if (afterId < line.size() && !isSpace(line[afterId]))
  {
    return false;
  }
  while (afterId < line.size() && isSpace(line[afterId]))
  {
    ++afterId;
  }
  if (afterId == line.size())
  {
    return false;
  }

  person.id = id;
  person.info = line.substr(afterId);
  return true;
}

bool kondrat::containsId(const PersonStorage & storage, size_t id)
{
  for (size_t i = 0; i < storage.size; ++i)
  {
    if (storage.data[i].id == id)
    {
      return true;
    }
  }
  return false;
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

void kondrat::reserve(PersonStorage & storage, size_t capacity)
{
  if (capacity <= storage.capacity)
  {
    return;
  }

  Person * newData = new Person[capacity];
  try
  {
    for (size_t i = 0; i < storage.size; ++i)
    {
      newData[i] = storage.data[i];
    }
  }
  catch (...)
  {
    delete[] newData;
    throw;
  }

  delete[] storage.data;
  storage.data = newData;
  storage.capacity = capacity;
}

void kondrat::pushBack(PersonStorage & storage, const Person & person)
{
  if (storage.size == storage.capacity)
  {
    reserve(storage, nextCapacity(storage.capacity));
  }

  storage.data[storage.size] = person;
  ++storage.size;
}

void kondrat::readPersons(std::istream & input, PersonStorage & storage, size_t & ignored)
{
  std::string line;
  while (std::getline(input, line))
  {
    Person person = {};
    if (!parsePerson(line, person) || containsId(storage, person.id))
    {
      ++ignored;
      continue;
    }
    pushBack(storage, person);
  }
}

void kondrat::printPersons(std::ostream & output, const PersonStorage & storage)
{
  for (size_t i = 0; i < storage.size; ++i)
  {
    output << storage.data[i].id << ' ' << storage.data[i].info << '\n';
  }
}

int main(int argc, char ** argv)
{}
