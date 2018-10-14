#pragma once

#include <memory>

struct Scene
{
  // returns false if the scene is finished
  virtual bool tick(double clock) = 0;
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

