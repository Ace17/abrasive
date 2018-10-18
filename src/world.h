// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Level object definition. Shared by the scenes.

#pragma once

#include "vec3.h"

#include <vector>

struct World
{
  std::vector<Vec3> path;
};

World loadLevel(const char* path);

