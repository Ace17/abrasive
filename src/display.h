#pragma once

#include <memory>

struct Display
{
  virtual ~Display() = default;
  virtual void update() = 0;
};

std::unique_ptr<Display> createDisplay();

