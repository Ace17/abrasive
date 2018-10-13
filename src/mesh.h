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

