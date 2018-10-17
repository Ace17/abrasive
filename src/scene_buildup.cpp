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
#include "timeline.h"
#include "world.h"

#include <vector>
#include <cmath>

using namespace std;

namespace
{
const vector<Event> timeline =
{
  { 0.00, "Bass Starts" },
  { 4.00, "Bass continues" },
};

static double frac(double val)
{
  return val - floor(val);
}

struct BuildupScene : Scene
{
  BuildupScene(Display* display, World* world) :
    m_display(display),
    m_world(world)
  {
    m_timeline.events = timeline;
  }

  void tick(double clock) override
  {
    m_timeline.update(clock, m_display);

    if(frac(clock * 0.25) < 0.01)
      m_display->pulse();

    updateState(clock);
    pushActors();
  }

  static Vec3 blend(Vec3 a, Vec3 b, float alpha)
  {
    return a * (1 - alpha) + b * alpha;
  }

  void updateState(double now)
  {
    auto a = m_world->locators.at("loc.000");
    auto b = m_world->locators.at("loc.003");
    Vec3 pos = blend(a, b, now / 16);
    auto t = now;
    Vec3 up;
    up.x = sin(t * 0.1);
    up.y = 0;
    up.z = cos(t * 0.1);

    m_display->setLightPos(pos + Vec3 { 0, float(min(now, 10.0)), 0 });
    m_display->setAmbientLight(now * 0.01);
    m_display->setCamera(pos, { 0, 1, 0 }, up);
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
  World* const m_world;
  Timeline m_timeline;
};

Scene* create(Display* display, World* world)
{
  return new BuildupScene(display, world);
}

static int registered = registerScene("buildup", &create);
}

