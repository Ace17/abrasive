// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Level object definition. Shared by the scenes.
#include "world.h"
#include "error.h"

#include <fstream>
#include <map>

using namespace std;

World loadLevel(const char* path)
{
  World w;
  ifstream fp(path, ios::binary);

  if(!fp.is_open())
    Fail("Can't open level file: '%s'", path);

  std::map<std::string, Vec3> locators;

  string line;

  while(getline(fp, line))
  {
    if(line.empty() || line[0] == '#')
      continue;

    char name[256] {};
    Vec3 pos;
    int count = sscanf(line.c_str(),
                       "%s %f %f %f",
                       name,
                       &pos.x,
                       &pos.y,
                       &pos.z
                       );

    if(count != 4)
      Fail("Invalid line in level file: '%s'", line.c_str());

    locators[name] = pos;
  }

  for(auto& loc : locators)
    w.path.push_back(loc.second);

  return w;
}

