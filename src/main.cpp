#include "SDL.h" // SDL_Delay
#include <cstdio>
#include <vector>

using namespace std;

auto const BPM = 120.0;

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

int startTime;

// in beats
double getClock()
{
  static const int startTime = SDL_GetTicks();
  return (SDL_GetTicks() - startTime) * BPM / 60000.0;
}

int main()
{
  int curr = 0;

  while(1)
  {
    auto now = getClock();
    if(curr + 1 < (int)timeline.size() && now >= timeline[curr+1].clockTime)
    {
      curr++;
      printf("%s\n", timeline[curr].text);
    }
    SDL_Delay(1);
  }
  return 0;
}

