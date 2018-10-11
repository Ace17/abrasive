#include <cstdio>
#include <cstdint>
#include <vector>

using namespace std;

auto const BPM = 85.0;

struct Event
{
  double clockTime; // in beats
  const char* text = "";
};

const vector<Event> timeline =
{
  { 0, "" },
  { 1, "A" },
  { 2, "B" },
  { 3, "C" },
  { 4, "D" },
  { 10, "hello" },
  { 20, "world" },
} ;

// in beats
extern int64_t getTicks();

double getClock()
{
  static const int startTime = getTicks();
  return (getTicks() - startTime) * BPM / 60000.0;
}

struct State
{
  int curr = 0;

  void tick()
  {
    auto now = getClock();
    if(curr + 1 < (int)timeline.size() && now >= timeline[curr+1].clockTime)
    {
      curr++;
      printf("%s\n", timeline[curr].text);
    }
  }
};

///////////////////////////////////////////////////////////////////////////////

#include "SDL.h" // SDL_Delay, SDL_GetTicks

int64_t getTicks()
{
  return SDL_GetTicks();
}

int main()
{
  State s;
  while(1)
  {
    s.tick();
    SDL_Delay(1);
  }
  return 0;
}

