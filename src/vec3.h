// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Basic 3D vector class
#pragma once
#include <cmath> // sqrt

struct Vec3
{
  float x = 0;
  float y = 0;
  float z = 0;
};

inline
Vec3 operator - (Vec3 a, Vec3 b)
{
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;
  return a;
}

inline
Vec3 operator + (Vec3 a, Vec3 b)
{
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

inline
Vec3 operator * (Vec3 v, float f)
{
  v.x *= f;
  v.y *= f;
  v.z *= f;
  return v;
}

inline
double dotProduct(Vec3 a, Vec3 b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline
Vec3 normalize(Vec3 vec)
{
  auto const magnitude = dotProduct(vec, vec);
  return vec * (1.0 / sqrt(magnitude));
}

