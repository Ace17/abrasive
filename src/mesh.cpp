#include "mesh.h"
#include "error.h"

#include <string>
#include <fstream>

using namespace std;

Mesh loadMesh(const char* path)
{
  auto fp = ifstream(path, ios::binary);

  if(!fp.is_open())
    Fail("Can't open model file: '%s'", path);

  Mesh mesh;
  string line;

  while(getline(fp, line))
  {
    if(line.empty() || line[0] == '#')
      continue;

    Vertex vertex;
    int count = sscanf(line.c_str(),
                       "%f %f %f - %f %f %f - %f %f - %f %f",
                       &vertex.x, &vertex.y, &vertex.z,
                       &vertex.nx, &vertex.ny, &vertex.nz,
                       &vertex.u1, &vertex.v1,
                       &vertex.u2, &vertex.v2);

    if(count != 10)
      Fail("Invalid line in mesh file: '%s'", line.c_str());

    mesh.vertices.push_back(vertex);
  }

  return mesh;
}
