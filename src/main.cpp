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
double getClock(int64_t ticks)
{
  static const int startTime = ticks;
  return (ticks - startTime) * BPM / 60000.0;
}

struct State
{
  int curr = 0;

  void tick(int64_t ticks)
  {
    auto now = getClock(ticks);
    if(curr + 1 < (int)timeline.size() && now >= timeline[curr+1].clockTime)
    {
      curr++;
      printf("%s\n", timeline[curr].text);
    }
  }
};

///////////////////////////////////////////////////////////////////////////////

#include "SDL.h" // SDL_Delay, SDL_GetTicks

static State g_state;

void init()
{
  SDL_Init(SDL_INIT_EVERYTHING);
}

void destroy()
{
  SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
  init();

  bool keepGoing = true;

  auto processEvent = [&](SDL_Event event)
  {
    if(event.type == SDL_QUIT)
      keepGoing = false;
  };

  while(keepGoing)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event))
      processEvent(event);

    g_state.tick(SDL_GetTicks());
    SDL_Delay(1);
  }

  destroy();
  return 0;
}

