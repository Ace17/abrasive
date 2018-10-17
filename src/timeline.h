// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
#include "display.h"

#include <vector>

struct Event
{
  double clockTime; // in beats
  const char* text = "";
};

struct Timeline
{
  std::vector<Event> events;
  int curr = -1;

  void update(double now, Display* display)
  {
    while(curr + 1 < (int)events.size() && now >= events[curr + 1].clockTime)
    {
      curr++;
      display->showText(events[curr].text);
    }
  }
};

