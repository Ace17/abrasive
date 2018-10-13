#include "file.h"
#include "error.h"

#include <fstream>

using namespace std;

template<typename T>
std::vector<T> load(const char* path)
{
  auto fp = ifstream(path, ios::binary);

  if(!fp.is_open())
    Fail("Can't open music file: '%s'", path);

  fp.seekg(0, ios::end);
  auto size = fp.tellg();
  fp.seekg(0, ios::beg);

  vector<T> buf(size / sizeof(T));
  fp.read((char*)buf.data(), buf.size() * sizeof(T));

  return buf;
}

template std::vector<uint8_t> load<uint8_t>(const char*);
template std::vector<int16_t> load<int16_t>(const char*);

