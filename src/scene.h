#pragma once

#include <memory>

struct Scene
{
  virtual void tick(double clock) = 0;
};

enum
{
  MODEL_BOX,
  MODEL_ROOM,
};

enum
{
  SHADER_BASIC,
};

struct Display;
typedef Scene* SceneCreationFunc (Display*);

int registerScene(std::string name, SceneCreationFunc* func);
std::unique_ptr<Scene> createScene(std::string name, Display*);

