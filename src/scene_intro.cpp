// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// The initial scene where the camera is still, and a
// rotating light is hinting the shaft ahead.
#include "scene.h"
#include "display.h"
#include "vec3.h"
#include "timeline.h"

#include <vector>
#include <cmath>

using namespace std;

namespace
{
const vector<Event> timeline =
{
  { 0.00, "0 - Start" },
  { 4.00, "4 - " },
  { 8.00, "8 - " },
  { 12.00, "12 - " },
};

struct InitScene : Scene
{
  InitScene(Display* display) :
    m_display(display)
  {
    m_timeline.events = timeline;
  }

  void tick(double clock) override
  {
    updateState(clock);
    pushActors();
  }

  void updateState(double now)
  {
    m_timeline.update(now, m_display);

    Vec3 lightPos;
    lightPos.x = cos(now * 2.0 * M_PI * 0.25) * 3.0;
    lightPos.y = sin(now * 2.0 * M_PI * 0.25) * 3.0;
    lightPos.z = 0.0;
    m_display->setAmbientLight(0);
    m_display->setLightPos(lightPos);
    m_display->setCamera(m_pos, { 0, 1, 0 }, { 0, 0, 1 });
  }

  void pushActors()
  {
    Actor actor;
    actor.model = MODEL_ROOM;
    actor.shader = SHADER_BASIC;
    m_display->pushActor(actor);
  }

private:
  Vec3 m_pos = { 0, -5, 0 };
  Display* const m_display;
  Timeline m_timeline;
};

Scene* create(Display* display)
{
  return new InitScene(display);
}

static int registered = registerScene("intro", &create);
}

