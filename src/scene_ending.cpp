// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// The last scene: infinite loop in corridor
#include "scene.h"
#include "display.h"
#include "vec3.h"

#include <vector>
#include <cmath>

using namespace std;

namespace
{
static double frac(double val)
{
  return val - floor(val);
}

struct EndingScene : Scene
{
  EndingScene(Display* display) :
    m_display(display)
  {
  }

  void tick(double clock) override
  {
    if(frac(clock * 0.25) < 0.01)
      m_display->pulse();

    updateState(clock);
    pushActors();
  }

  void updateState(double now)
  {
    m_display->setAmbientLight(0.5 + 0.2 * sin(now * 0.2 * 2.0 * M_PI));

    auto tilt = sin(now * 0.1 * 2.0 * M_PI) * 0.1 + M_PI * 0.5;

    Vec3 pos = { -20, 10, (float)(sin(now * 0.1 * 2.0 * M_PI) * 0.3) };

    m_display->setCamera(pos, { 0, -1, 0 }, { (float)cos(tilt), 0, (float)sin(tilt) });
  }

  void pushActors()
  {
    Actor actor;
    actor.model = MODEL_ROOM;
    actor.shader = SHADER_BASIC;
    m_display->pushActor(actor);
  }

private:
  Display* const m_display;
};

Scene* create(Display* display, World*)
{
  return new EndingScene(display);
}

static int registered = registerScene("ending", &create);
}

