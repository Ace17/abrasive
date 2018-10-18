// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Minimal linear algebra stuff
#pragma once

#include <cassert>
#include <cmath> // tan, sqrt
#include "vec3.h"

///////////////////////////////////////////////////////////////////////////////
// Vec3 operations

inline
Vec3 crossProduct(Vec3 a, Vec3 b)
{
  Vec3 r;
  r.x = a.y * b.z - a.z * b.y;
  r.y = a.z * b.x - a.x * b.z;
  r.z = a.x * b.y - a.y * b.x;
  return r;
}

///////////////////////////////////////////////////////////////////////////////
// Mat4

struct Mat4
{
  struct Col4
  {
    float elements[4] {};

    float const & operator [] (int i) const { return elements[i]; }
    float & operator [] (int i) { return elements[i]; }
  };

  const Col4 & operator [] (int i) const { return data[i]; }

  Col4 & operator [] (int i) { return data[i]; }

  Col4 data[4] {};
};

inline
Mat4 operator * (Mat4 const& A, Mat4 const& B)
{
  Mat4 r;

  for(int row = 0; row < 4; ++row)
  {
    for(int col = 0; col < 4; ++col)
    {
      double sum = 0;

      for(int k = 0; k < 4; ++k)
        sum += A[k][row] * B[col][k];

      r[col][row] = sum;
    }
  }

  return r;
}

inline
Mat4 translate(Vec3 v)
{
  Mat4 r;
  r[0][0] = 1;
  r[1][1] = 1;
  r[2][2] = 1;
  r[3][0] = v.x;
  r[3][1] = v.y;
  r[3][2] = v.z;
  r[3][3] = 1;
  return r;
}

inline
Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up)
{
  auto f = normalize(center - eye);
  auto s = normalize(crossProduct(f, up));
  auto u = crossProduct(s, f);

  Mat4 r;
  r[0][0] = s.x;
  r[1][0] = s.y;
  r[2][0] = s.z;
  r[0][1] = u.x;
  r[1][1] = u.y;
  r[2][1] = u.z;
  r[0][2] = -f.x;
  r[1][2] = -f.y;
  r[2][2] = -f.z;
  r[3][0] = -dotProduct(s, eye);
  r[3][1] = -dotProduct(u, eye);
  r[3][2] = dotProduct(f, eye);
  r[3][3] = 1;
  return r;
}

inline
Mat4 perspective(float fovy, float aspect, float zNear, float zFar)
{
  assert(aspect != 0.0);
  assert(zFar != zNear);

  auto const rad = fovy;
  auto const tanHalfFovy = tan(rad / 2.0);

  Mat4 r;
  r[0][0] = 1.0 / (aspect * tanHalfFovy);
  r[1][1] = 1.0 / (tanHalfFovy);
  r[2][2] = -(zFar + zNear) / (zFar - zNear);
  r[2][3] = -1.0;
  r[3][2] = -(2.0 * zFar * zNear) / (zFar - zNear);
  return r;
}

