#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <utils.hpp>

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

  bool parseArgs(int argc, char ** argv, ProgramArgs & args);
  void initStorage(PersonStorage & storage);
  void destroyStorage(PersonStorage & storage);
  bool isBlank(const std::string & line);
  bool parsePerson(const std::string & line, Person & person);
  bool containsId(const PersonStorage & storage, size_t id);
  void reserve(PersonStorage & storage, size_t capacity);
  void pushBack(PersonStorage & storage, const Person & person);
  void readPersons(std::istream & input, PersonStorage & storage, size_t & ignored);
  void printPersons(std::ostream & output, const PersonStorage & storage);
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

bool kondrat::isBlank(const std::string & line)
{
  for (size_t i = 0; i < line.size(); ++i)
  {
    if (!isSpace(line[i]))
    {
      return false;
    }
  }
  return true;
}

bool kondrat::parsePerson(const std::string & line, Person & person)
{
  size_t first = 0;
  while (first < line.size() && isSpace(line[first]))
  {
    ++first;
  }
  size_t id = 0;
  if (!readSizeT(line, first, id))
  {
    return false;
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
    if (isBlank(line))
    {
      continue;
    }

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
{
  if (argc > 3)
  {
    return 0;
  }

  kondrat::ProgramArgs args = {};
  if (!kondrat::parseArgs(argc, argv, args))
  {
    return 1;
  }

  std::ifstream inputFile;
  std::istream * input = &std::cin;
  if (args.input != nullptr)
  {
    inputFile.open(args.input);
    if (!inputFile)
    {
      return 2;
    }
    input = &inputFile;
  }

  kondrat::PersonStorage storage = {};
  kondrat::initStorage(storage);
  size_t ignored = 0;
  try
  {
    kondrat::readPersons(*input, storage, ignored);
  }
  catch (...)
  {
    kondrat::destroyStorage(storage);
    throw;
  }
  inputFile.close();

  std::ofstream outputFile;
  std::ostream * output = &std::cout;
  if (args.output != nullptr)
  {
    outputFile.open(args.output);
    if (!outputFile)
    {
      kondrat::destroyStorage(storage);
      return 2;
    }
    output = &outputFile;
  }

  try
  {
    kondrat::printPersons(*output, storage);
    if (storage.size != 0 || ignored != 0)
    {
      std::cerr << storage.size << ' ' << ignored << '\n';
    }
  }
  catch (...)
  {
    kondrat::destroyStorage(storage);
    throw;
  }
  kondrat::destroyStorage(storage);

  return 0;
}
