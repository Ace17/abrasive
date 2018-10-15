// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// The scene at the beginning where the camera starts moving
// and the bassline starts rolling.
#include "scene.h"
#include "display.h"
#include "vec3.h"

#include <vector>
#include <cmath>

using namespace std;

namespace
{
struct DebugScene : Scene
{
  DebugScene(Display* display) :
    m_display(display)
  {
  }

  void tick(double clock) override
  {
    updateState(clock);
    pushActors();
  }

  void updateState(double now)
  {
    Vec3 pos = { -1, -1, 2 };
    Vec3 up = { 0, 0, 1 };
    Vec3 dir = { 1, 1, -1 };

    Vec3 lightPos;
    lightPos.x = cos(now * 1.5) * 1.4;
    lightPos.y = sin(now * 1.5) * 1.4;
    lightPos.z = 1;

    m_display->setLightPos(lightPos);
    m_display->setAmbientLight(0.5);
    m_display->setCamera(pos, dir, up);
  }

  void pushActors()
  {
    Actor actor;
    actor.model = MODEL_BOX;
    actor.shader = SHADER_BASIC;
    m_display->pushActor(actor);
  }

private:
  Display* const m_display;
};

Scene* create(Display* display)
{
  return new DebugScene(display);
}

static int registered = registerScene("debug", &create);
}

