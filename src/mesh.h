// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Mesh (as seen by the display)
#pragma once

#include <vector>

struct Vertex
{
  float x, y, z; // position
  float nx, ny, nz; // normal
  float u1, v1; // for diffuse texture
  float u2, v2; // for lightmap texture
};

struct Mesh
{
  std::vector<Vertex> vertices;
  int texture;
};

Mesh loadMesh(const char* path);

