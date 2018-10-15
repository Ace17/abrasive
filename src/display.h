// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// high-level display interface (i.e as seen by the main app)
#pragma once

#include "vec3.h"
#include <memory>

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

  virtual void loadShader(int shaderId, const char* path) = 0;
  virtual void loadModel(int modelId, const char* path) = 0;

  virtual void pulse() = 0;
  virtual void setAmbientLight(float) = 0;
  virtual void setLightPos(Vec3 pos) = 0;
  virtual void setCamera(Vec3 pos, Vec3 forward, Vec3 up) = 0;

  virtual void showText(const char* msg) = 0;
  virtual void pushActor(Actor const& actor) = 0;
};

std::unique_ptr<Display> createDisplay();

