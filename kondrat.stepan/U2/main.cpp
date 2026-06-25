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
    bool described;
  };

  struct Meeting
  {
    size_t first;
    size_t second;
    size_t time;
  };

  struct MeetingInfo
  {
    size_t id;
    size_t time;
  };

  template< class T >
  struct Storage
  {
    T * data;
    size_t size;
    size_t capacity;
  };

  using PersonStorage = Storage< Person >;
  using MeetingStorage = Storage< Meeting >;

  struct ProgramArgs
  {
    const char * input;
    const char * data;
  };

  bool parseArgs(int argc, char ** argv, ProgramArgs & args);
  template< class T >
  void initStorage(Storage< T > & storage);
  template< class T >
  void destroyStorage(Storage< T > & storage);
  template< class T >
  void reserve(Storage< T > & storage, size_t capacity);
  template< class T >
  void pushBack(Storage< T > & storage, const T & value);
  template< class T, class Compare >
  void sort(Storage< T > & storage, Compare compare);
  bool lessSize(size_t lhs, size_t rhs);
  bool lessMeetingInfo(const MeetingInfo & lhs, const MeetingInfo & rhs);
  bool isBlank(const std::string & line);
  void skipSpaces(const std::string & line, size_t & pos);
  bool readToken(const std::string & line, size_t & pos, std::string & token);
  bool readNumber(const std::string & line, size_t & pos, size_t & number);
  bool readQuoted(const std::string & line, size_t & pos, std::string & value);
  bool isLineEnd(const std::string & line, size_t pos);
  bool parsePerson(const std::string & line, Person & person);
  Person * findPerson(PersonStorage & storage, size_t id);
  const Person * findPerson(const PersonStorage & storage, size_t id);
  size_t findPersonIndex(const PersonStorage & storage, size_t id);
  bool containsPerson(const PersonStorage & storage, size_t id);
  bool isExistingPerson(const PersonStorage & persons, size_t id);
  void ensureAnonPerson(PersonStorage & storage, size_t id);
  void removePerson(PersonStorage & storage, size_t index);
  void readPersons(std::istream & input, PersonStorage & storage);
  bool parseMeeting(const std::string & line, Meeting & meeting);
  bool readMeetings(std::istream & input, MeetingStorage & meetings, PersonStorage & persons);
  void printAnons(std::ostream & output, const PersonStorage & persons);
  bool printDesc(std::ostream & output, const PersonStorage & persons, size_t id);
  bool printMeetings(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t id);
  bool printMeetingsLess(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t id, size_t time);
  bool printMeetingsGreater(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t id, size_t time);
  bool printCommons(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t lhs, size_t rhs);
  bool redesc(PersonStorage & persons, size_t id, const std::string & desc);
  bool deanon(PersonStorage & persons, MeetingStorage & meetings, size_t anonId, size_t id);
  bool outPersons(const char * filename, const PersonStorage & persons);
  bool processCommand(const std::string & line, std::ostream & output, PersonStorage & persons, MeetingStorage & meetings);
  void processCommands(std::istream & input, std::ostream & output, PersonStorage & persons, MeetingStorage & meetings);
}

bool kondrat::parseArgs(int argc, char ** argv, ProgramArgs & args)
{
  args.input = nullptr;
  args.data = nullptr;
  if (argc < 2 || argc > 3)
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
    else if (startsWith(argv[i], "data:"))
    {
      if (args.data != nullptr || argv[i][5] == '\0')
      {
        return false;
      }
      args.data = argv[i] + 5;
    }
    else
    {
      return false;
    }
  }

  return args.data != nullptr;
}

template< class T >
void kondrat::initStorage(Storage< T > & storage)
{
  storage.data = nullptr;
  storage.size = 0;
  storage.capacity = 0;
}

template< class T >
void kondrat::destroyStorage(Storage< T > & storage)
{
  delete[] storage.data;
  initStorage(storage);
}

template< class T >
void kondrat::reserve(Storage< T > & storage, size_t capacity)
{
  if (capacity <= storage.capacity)
  {
    return;
  }

  T * newData = new T[capacity];
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

template< class T >
void kondrat::pushBack(Storage< T > & storage, const T & value)
{
  if (storage.size == storage.capacity)
  {
    reserve(storage, nextCapacity(storage.capacity));
  }

  storage.data[storage.size] = value;
  ++storage.size;
}

template< class T, class Compare >
void kondrat::sort(Storage< T > & storage, Compare compare)
{
  for (size_t i = 0; i < storage.size; ++i)
  {
    for (size_t j = i + 1; j < storage.size; ++j)
    {
      if (compare(storage.data[j], storage.data[i]))
      {
        const T tmp = storage.data[i];
        storage.data[i] = storage.data[j];
        storage.data[j] = tmp;
      }
    }
  }
}

bool kondrat::lessSize(size_t lhs, size_t rhs)
{
  return lhs < rhs;
}

bool kondrat::lessMeetingInfo(const MeetingInfo & lhs, const MeetingInfo & rhs)
{
  return lhs.id < rhs.id || (lhs.id == rhs.id && lhs.time < rhs.time);
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

void kondrat::skipSpaces(const std::string & line, size_t & pos)
{
  while (pos < line.size() && isSpace(line[pos]))
  {
    ++pos;
  }
}

bool kondrat::readToken(const std::string & line, size_t & pos, std::string & token)
{
  skipSpaces(line, pos);
  if (pos == line.size())
  {
    return false;
  }

  const size_t start = pos;
  while (pos < line.size() && !isSpace(line[pos]))
  {
    ++pos;
  }
  token = line.substr(start, pos - start);
  return true;
}

bool kondrat::readNumber(const std::string & line, size_t & pos, size_t & number)
{
  skipSpaces(line, pos);
  return readSizeT(line, pos, number);
}

bool kondrat::readQuoted(const std::string & line, size_t & pos, std::string & value)
{
  skipSpaces(line, pos);
  if (pos == line.size() || line[pos] != '"')
  {
    return false;
  }
  ++pos;
  const size_t start = pos;
  while (pos < line.size() && line[pos] != '"')
  {
    ++pos;
  }
  if (pos == line.size())
  {
    return false;
  }
  value = line.substr(start, pos - start);
  ++pos;
  return true;
}

bool kondrat::isLineEnd(const std::string & line, size_t pos)
{
  skipSpaces(line, pos);
  return pos == line.size();
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
  person.described = true;
  return true;
}

bool kondrat::containsPerson(const PersonStorage & storage, size_t id)
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

kondrat::Person * kondrat::findPerson(PersonStorage & storage, size_t id)
{
  const PersonStorage & constStorage = storage;
  return const_cast< Person * >(findPerson(constStorage, id));
}

const kondrat::Person * kondrat::findPerson(const PersonStorage & storage, size_t id)
{
  for (size_t i = 0; i < storage.size; ++i)
  {
    if (storage.data[i].id == id)
    {
      return storage.data + i;
    }
  }
  return nullptr;
}

size_t kondrat::findPersonIndex(const PersonStorage & storage, size_t id)
{
  for (size_t i = 0; i < storage.size; ++i)
  {
    if (storage.data[i].id == id)
    {
      return i;
    }
  }
  return storage.size;
}

void kondrat::ensureAnonPerson(PersonStorage & storage, size_t id)
{
  if (findPerson(storage, id) != nullptr)
  {
    return;
  }

  Person person = {};
  person.id = id;
  person.info = "";
  person.described = false;
  pushBack(storage, person);
}

bool kondrat::isExistingPerson(const PersonStorage & persons, size_t id)
{
  return findPerson(persons, id) != nullptr;
}

void kondrat::removePerson(PersonStorage & storage, size_t index)
{
  if (index >= storage.size)
  {
    return;
  }
  for (size_t i = index + 1; i < storage.size; ++i)
  {
    storage.data[i - 1] = storage.data[i];
  }
  --storage.size;
}

void kondrat::readPersons(std::istream & input, PersonStorage & storage)
{
  std::string line;
  while (std::getline(input, line))
  {
    if (isBlank(line))
    {
      continue;
    }

    Person person = {};
    if (!parsePerson(line, person) || containsPerson(storage, person.id))
    {
      continue;
    }
    pushBack(storage, person);
  }
}

bool kondrat::parseMeeting(const std::string & line, Meeting & meeting)
{
  size_t pos = 0;
  while (pos < line.size() && isSpace(line[pos]))
  {
    ++pos;
  }

  size_t first = 0;
  if (!readSizeT(line, pos, first) || pos == line.size() || !isSpace(line[pos]))
  {
    return false;
  }
  while (pos < line.size() && isSpace(line[pos]))
  {
    ++pos;
  }

  size_t second = 0;
  if (!readSizeT(line, pos, second) || pos == line.size() || !isSpace(line[pos]))
  {
    return false;
  }
  while (pos < line.size() && isSpace(line[pos]))
  {
    ++pos;
  }

  size_t time = 0;
  if (!readSizeT(line, pos, time))
  {
    return false;
  }
  while (pos < line.size() && isSpace(line[pos]))
  {
    ++pos;
  }
  if (pos != line.size())
  {
    return false;
  }

  meeting.first = first;
  meeting.second = second;
  meeting.time = time;
  return true;
}

bool kondrat::readMeetings(std::istream & input, MeetingStorage & meetings, PersonStorage & persons)
{
  std::string line;
  while (std::getline(input, line))
  {
    if (isBlank(line))
    {
      continue;
    }

    Meeting meeting = {};
    if (!parseMeeting(line, meeting))
    {
      return false;
    }
    if (meeting.first == meeting.second)
    {
      continue;
    }
    ensureAnonPerson(persons, meeting.first);
    ensureAnonPerson(persons, meeting.second);
    pushBack(meetings, meeting);
  }
  return true;
}

void kondrat::printAnons(std::ostream & output, const PersonStorage & persons)
{
  Storage< size_t > ids = {};
  initStorage(ids);
  try
  {
    for (size_t i = 0; i < persons.size; ++i)
    {
      if (!persons.data[i].described)
      {
        pushBack(ids, persons.data[i].id);
      }
    }
    sort(ids, lessSize);
    for (size_t i = 0; i < ids.size; ++i)
    {
      output << ids.data[i] << '\n';
    }
  }
  catch (...)
  {
    destroyStorage(ids);
    throw;
  }
  destroyStorage(ids);
}

bool kondrat::printDesc(std::ostream & output, const PersonStorage & persons, size_t id)
{
  const Person * person = findPerson(persons, id);
  if (person == nullptr)
  {
    return false;
  }

  if (person->described)
  {
    output << person->info << '\n';
  }
  else
  {
    output << "<ANON>\n";
  }
  return true;
}

bool kondrat::printMeetings(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t id)
{
  if (!isExistingPerson(persons, id))
  {
    return false;
  }

  Storage< MeetingInfo > result = {};
  initStorage(result);
  try
  {
    for (size_t i = 0; i < meetings.size; ++i)
    {
      MeetingInfo info = {};
      if (meetings.data[i].first == id)
      {
        info.id = meetings.data[i].second;
        info.time = meetings.data[i].time;
        pushBack(result, info);
      }
      else if (meetings.data[i].second == id)
      {
        info.id = meetings.data[i].first;
        info.time = meetings.data[i].time;
        pushBack(result, info);
      }
    }
    sort(result, lessMeetingInfo);
    for (size_t i = 0; i < result.size; ++i)
    {
      output << result.data[i].id << ' ' << result.data[i].time << '\n';
    }
  }
  catch (...)
  {
    destroyStorage(result);
    throw;
  }
  destroyStorage(result);
  return true;
}

bool kondrat::printMeetingsLess(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t id, size_t time)
{
  if (!isExistingPerson(persons, id))
  {
    return false;
  }

  Storage< MeetingInfo > result = {};
  initStorage(result);
  try
  {
    for (size_t i = 0; i < meetings.size; ++i)
    {
      MeetingInfo info = {};
      if (meetings.data[i].first == id && meetings.data[i].time < time)
      {
        info.id = meetings.data[i].second;
        info.time = meetings.data[i].time;
        pushBack(result, info);
      }
      else if (meetings.data[i].second == id && meetings.data[i].time < time)
      {
        info.id = meetings.data[i].first;
        info.time = meetings.data[i].time;
        pushBack(result, info);
      }
    }
    sort(result, lessMeetingInfo);
    for (size_t i = 0; i < result.size; ++i)
    {
      output << result.data[i].id << ' ' << result.data[i].time << '\n';
    }
  }
  catch (...)
  {
    destroyStorage(result);
    throw;
  }
  destroyStorage(result);
  return true;
}

bool kondrat::printMeetingsGreater(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t id, size_t time)
{
  if (!isExistingPerson(persons, id))
  {
    return false;
  }

  Storage< MeetingInfo > result = {};
  initStorage(result);
  try
  {
    for (size_t i = 0; i < meetings.size; ++i)
    {
      MeetingInfo info = {};
      if (meetings.data[i].first == id && meetings.data[i].time > time)
      {
        info.id = meetings.data[i].second;
        info.time = meetings.data[i].time;
        pushBack(result, info);
      }
      else if (meetings.data[i].second == id && meetings.data[i].time > time)
      {
        info.id = meetings.data[i].first;
        info.time = meetings.data[i].time;
        pushBack(result, info);
      }
    }
    sort(result, lessMeetingInfo);
    for (size_t i = 0; i < result.size; ++i)
    {
      output << result.data[i].id << ' ' << result.data[i].time << '\n';
    }
  }
  catch (...)
  {
    destroyStorage(result);
    throw;
  }
  destroyStorage(result);
  return true;
}

bool kondrat::printCommons(std::ostream & output, const PersonStorage & persons, const MeetingStorage & meetings, size_t lhs, size_t rhs)
{
  if (!isExistingPerson(persons, lhs) || !isExistingPerson(persons, rhs))
  {
    return false;
  }

  Storage< size_t > ids = {};
  initStorage(ids);
  try
  {
    for (size_t i = 0; i < meetings.size; ++i)
    {
      size_t candidate = 0;
      bool found = false;
      if (meetings.data[i].first == lhs)
      {
        candidate = meetings.data[i].second;
        found = true;
      }
      else if (meetings.data[i].second == lhs)
      {
        candidate = meetings.data[i].first;
        found = true;
      }
      if (!found)
      {
        continue;
      }

      bool rhsMet = false;
      for (size_t j = 0; j < meetings.size; ++j)
      {
        if ((meetings.data[j].first == rhs && meetings.data[j].second == candidate)
          || (meetings.data[j].second == rhs && meetings.data[j].first == candidate))
        {
          rhsMet = true;
        }
      }
      if (rhsMet)
      {
        bool exists = false;
        for (size_t k = 0; k < ids.size; ++k)
        {
          if (ids.data[k] == candidate)
          {
            exists = true;
          }
        }
        if (!exists)
        {
          pushBack(ids, candidate);
        }
      }
    }
    sort(ids, lessSize);
    for (size_t i = 0; i < ids.size; ++i)
    {
      output << ids.data[i] << '\n';
    }
  }
  catch (...)
  {
    destroyStorage(ids);
    throw;
  }
  destroyStorage(ids);
  return true;
}

bool kondrat::redesc(PersonStorage & persons, size_t id, const std::string & desc)
{
  Person * person = findPerson(persons, id);
  if (person == nullptr)
  {
    return false;
  }

  person->info = desc;
  person->described = true;
  return true;
}

bool kondrat::deanon(PersonStorage & persons, MeetingStorage & meetings, size_t anonId, size_t id)
{
  const size_t anonIndex = findPersonIndex(persons, anonId);
  Person * target = findPerson(persons, id);
  if (anonIndex == persons.size || target == nullptr || persons.data[anonIndex].described || !target->described)
  {
    return false;
  }

  for (size_t i = 0; i < meetings.size; ++i)
  {
    if (meetings.data[i].first == anonId)
    {
      meetings.data[i].first = id;
    }
    if (meetings.data[i].second == anonId)
    {
      meetings.data[i].second = id;
    }
  }

  size_t write = 0;
  for (size_t i = 0; i < meetings.size; ++i)
  {
    if (meetings.data[i].first != meetings.data[i].second)
    {
      meetings.data[write] = meetings.data[i];
      ++write;
    }
  }
  meetings.size = write;
  removePerson(persons, anonIndex);
  return true;
}

bool kondrat::outPersons(const char * filename, const PersonStorage & persons)
{
  std::ofstream file(filename);
  if (!file)
  {
    return false;
  }
  for (size_t i = 0; i < persons.size; ++i)
  {
    if (persons.data[i].described)
    {
      file << persons.data[i].id << ' ' << persons.data[i].info << '\n';
    }
  }
  return static_cast< bool >(file);
}

bool kondrat::processCommand(const std::string & line, std::ostream & output, PersonStorage & persons, MeetingStorage & meetings)
{
  size_t pos = 0;
  std::string command = "";
  if (!readToken(line, pos, command))
  {
    return true;
  }

  if (command == "anons")
  {
    if (!isLineEnd(line, pos))
    {
      return false;
    }
    printAnons(output, persons);
    return true;
  }
  if (command == "desc")
  {
    size_t id = 0;
    if (!readNumber(line, pos, id) || !isLineEnd(line, pos))
    {
      return false;
    }
    return printDesc(output, persons, id);
  }
  if (command == "meets")
  {
    size_t id = 0;
    if (!readNumber(line, pos, id) || !isLineEnd(line, pos))
    {
      return false;
    }
    return printMeetings(output, persons, meetings, id);
  }
  if (command == "commons")
  {
    size_t lhs = 0;
    size_t rhs = 0;
    if (!readNumber(line, pos, lhs) || !readNumber(line, pos, rhs) || !isLineEnd(line, pos))
    {
      return false;
    }
    return printCommons(output, persons, meetings, lhs, rhs);
  }
  if (command == "less" || command == "greater")
  {
    size_t time = 0;
    size_t id = 0;
    if (!readNumber(line, pos, time) || !readNumber(line, pos, id) || !isLineEnd(line, pos))
    {
      return false;
    }
    return command == "less"
      ? printMeetingsLess(output, persons, meetings, id, time)
      : printMeetingsGreater(output, persons, meetings, id, time);
  }
  if (command == "redesc")
  {
    size_t id = 0;
    std::string desc = "";
    if (!readNumber(line, pos, id) || !readQuoted(line, pos, desc) || !isLineEnd(line, pos))
    {
      return false;
    }
    return redesc(persons, id, desc);
  }
  if (command == "deanon")
  {
    size_t anonId = 0;
    size_t id = 0;
    if (!readNumber(line, pos, anonId) || !readNumber(line, pos, id) || !isLineEnd(line, pos))
    {
      return false;
    }
    return deanon(persons, meetings, anonId, id);
  }
  if (command == "out-persons")
  {
    std::string filename = "";
    if (!readToken(line, pos, filename) || !isLineEnd(line, pos))
    {
      return false;
    }
    return outPersons(filename.c_str(), persons);
  }

  return false;
}

void kondrat::processCommands(std::istream & input, std::ostream & output, PersonStorage & persons, MeetingStorage & meetings)
{
  std::string line;
  while (std::getline(input, line))
  {
    if (isBlank(line))
    {
      continue;
    }
    if (!processCommand(line, output, persons, meetings))
    {
      output << "<INVALID COMMAND>\n";
    }
  }
}

int main(int argc, char ** argv)
{
  kondrat::ProgramArgs args = {};
  if (!kondrat::parseArgs(argc, argv, args))
  {
    std::cerr << "<INVALID ARGUMENTS>\n";
    return 1;
  }

  std::ifstream personFile;
  std::istream * personInput = nullptr;
  if (args.input != nullptr)
  {
    personFile.open(args.input);
    if (!personFile)
    {
      std::cerr << "<FILE ERROR>\n";
      return 2;
    }
    personInput = &personFile;
  }

  std::ifstream meetingFile(args.data);
  if (!meetingFile)
  {
    std::cerr << "<FILE ERROR>\n";
    return 2;
  }

  kondrat::PersonStorage persons = {};
  kondrat::MeetingStorage meetings = {};
  kondrat::initStorage(persons);
  kondrat::initStorage(meetings);
  try
  {
    if (personInput != nullptr)
    {
      kondrat::readPersons(*personInput, persons);
    }
    if (!kondrat::readMeetings(meetingFile, meetings, persons))
    {
      kondrat::destroyStorage(meetings);
      kondrat::destroyStorage(persons);
      std::cerr << "<INVALID DATA>\n";
      return 3;
    }
    kondrat::processCommands(std::cin, std::cout, persons, meetings);
  }
  catch (...)
  {
    kondrat::destroyStorage(meetings);
    kondrat::destroyStorage(persons);
    throw;
  }

  kondrat::destroyStorage(meetings);
  kondrat::destroyStorage(persons);
  return 0;
}
