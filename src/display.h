#pragma once

#include <memory>

struct Display
{
  virtual ~Display() = default;
  virtual void update() = 0;
  virtual void showText(const char* msg) = 0;
};

std::unique_ptr<Display> createDisplay();

