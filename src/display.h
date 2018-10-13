#pragma once

#include <memory>

struct Vec3
{
  float x = 0;
  float y = 0;
  float z = 0;
};

struct Actor
{
  Vec3 pos {};
  int model = 0;
  int shader = 0;
};

struct Display
{
  virtual ~Display() = default;
  virtual void update() = 0;
  virtual void showText(const char* msg) = 0;
  virtual void pulse() = 0;
  virtual void loadShader(int shaderId, const char* path) = 0;
  virtual void loadModel(int modelId, const char* path) = 0;
  virtual void pushActor(Actor const& actor) = 0;
};

std::unique_ptr<Display> createDisplay();

