// =============================================================================
// A B R A S I V E
//
// by Sebastien Alaiwan
//
// =============================================================================
#include <cstdio>
#include <cstdint>
#include <vector>
#include <memory>

#include "audio.h"
#include "display.h"

using namespace std;

auto const BPM = 72.5;

enum
{
  MODEL_BOX,
  MODEL_ROOM,
};

enum
{
  SHADER_BASIC,
};

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
  State() :
    m_display(createDisplay()),
    m_audio(createAudio("assets/music.pcm"))
  {
    m_display->loadShader(SHADER_BASIC, "assets/shaders/basic");
    m_display->loadModel(MODEL_BOX, "assets/meshes/box.mesh");
    m_display->loadModel(MODEL_ROOM, "assets/meshes/room.mesh");
  }

  void tick()
  {
    auto seconds = m_audio->getTime();
    auto now = getClock(seconds);

    while(curr + 1 < (int)timeline.size() && now >= timeline[curr + 1].clockTime)
    {
      curr++;
      m_display->showText(timeline[curr].text);
      m_display->pulse();
    }

    if(int(now) % 2 == 0)
    {
      Actor actor;
      actor.model = MODEL_BOX;
      actor.shader = SHADER_BASIC;
      m_display->pushActor(actor);
    }

    {
      Actor actor;
      actor.model = MODEL_ROOM;
      actor.shader = SHADER_BASIC;
      m_display->pushActor(actor);
    }

    if(now >= 16)
      m_display->setAmbientLight(now * 0.01);
    m_display->update();
  }

private:
  int curr = -1;

  unique_ptr<Audio> m_audio;
  unique_ptr<Display> m_display;
};

///////////////////////////////////////////////////////////////////////////////
// Here begins the SDL-dependent part

#include "SDL.h"

static unique_ptr<State> g_state;

void init()
{
  SDL_InitSubSystem(SDL_INIT_EVENTS);
  g_state = make_unique<State>();
}

void destroy()
{
  g_state.reset();
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

    g_state->tick();
    SDL_Delay(1);
  }

  destroy();
  return 0;
}

