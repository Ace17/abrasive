// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Scene: a part of the full scenario
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

