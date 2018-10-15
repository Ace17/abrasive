// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// The audio layer, as seen by the main app.

#pragma once

#include <memory>

struct Audio
{
  virtual ~Audio() = default;
  virtual double getTime() = 0;
};

std::unique_ptr<Audio> createAudio(const char* musicPath);

