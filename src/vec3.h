#pragma once

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

