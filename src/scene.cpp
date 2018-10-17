// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// Pluggable factory for scenes

#include "scene.h"

#include "error.h"
#include <map>
#include <string>

using namespace std;

static map<std::string, SceneCreationFunc*> g_scenes;

int registerScene(std::string name, SceneCreationFunc* func)
{
  if(g_scenes.find(name) != g_scenes.end())
    Fail("a scene named '%s' is already registered", name);

  g_scenes[name] = func;
  return 0;
}

unique_ptr<Scene> createScene(std::string name, Display* display, World* world)
{
  if(g_scenes.find(name) == g_scenes.end())
    Fail("unknown scene '%s'", name.c_str());

  return unique_ptr<Scene>(g_scenes[name] (display, world));
}

