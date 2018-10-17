// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// NULL audio
#include "audio.h"

#include "SDL.h"

using namespace std;

struct NullAudio : Audio
{
  NullAudio()
  {
    m_timeInTicks = SDL_GetTicks();
  }

  double getTime()
  {
    return  (SDL_GetTicks() - m_timeInTicks) / 1000.0;
  }

private:
  int64_t m_timeInTicks;
};

unique_ptr<Audio> createAudio(const char*)
{
  return make_unique<NullAudio>();
}

