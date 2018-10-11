#include <cstdio>
#include <cstdint>
#include <vector>
#include <memory>

using namespace std;

auto const BPM = 72.5;

struct Event
{
  double clockTime; // in beats
  const char* text = "";
};

const vector<Event> timeline =
{
  { 0.00, "0 - Start" },
  { 4.00, "4 - " },
  { 8.00, "8 - " },
  { 12.00, "8 - " },
  { 16.00, "16 - " },
  { 20.00, "20 - " },
  { 24.00, "24 - " },
  { 28.00, "28 - " },
  { 32.00, "32 - Drumkit is rolling" },
  { 48.00, "48 - Saw FX" },
  { 64.00, "64 - Breakdown Sawtooth porta" },
  { 96.00, "96 - End" },
};

// in beats
double getClock(double seconds)
{
  return seconds * (BPM / 60);
}

struct State
{
  int curr = -1;

  void tick(double seconds)
  {
    auto now = getClock(seconds);

    while(curr + 1 < (int)timeline.size() && now >= timeline[curr + 1].clockTime)
    {
      curr++;
      printf("\r%s                     ", timeline[curr].text);
      fflush(stdout);
    }
  }
};

///////////////////////////////////////////////////////////////////////////////

#include "SDL.h"
#include "audio.h"
#include "display.h"

static State g_state;

unique_ptr<Audio> g_Audio;
unique_ptr<Display> g_Display;

void init()
{
  SDL_InitSubSystem(SDL_INIT_EVENTS);
  g_Display = createDisplay();
  g_Audio = createAudio();
}

void destroy()
{
  g_Audio.reset();
  g_Display.reset();
  SDL_QuitSubSystem(SDL_INIT_EVENTS);
  fprintf(stderr, "OK\n");
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
  init();

  bool keepGoing = true;

  while(keepGoing)
  {
    SDL_Event event;

    while(SDL_PollEvent(&event))
      if(event.type == SDL_QUIT)
        keepGoing = false;

    g_state.tick(g_Audio->getTime());
    g_Display->update();
    SDL_Delay(1);
  }

  destroy();
  return 0;
}

