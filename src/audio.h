#pragma once

#include <memory>

struct Audio
{
  virtual ~Audio() = default;
  virtual double getTime() = 0;
};

std::unique_ptr<Audio> createAudio();

