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

#include "SDL.h"

static State g_state;

void Fail(const char* fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  char buffer[4096] {};
  vsnprintf(buffer, sizeof buffer - 1, fmt, va);
  fprintf(stderr, "Fatal: %s\n", buffer);
  exit(1);
}

int64_t g_totalSamples;

void mixAudio(void* userParam, Uint8* buffer, int size)
{
  memset(buffer, 0, size);
  g_totalSamples += size / 4;
}

int64_t getAudioTime()
{
  return g_totalSamples * 1000.0 / 22050.0;
}

void init()
{
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_AudioSpec requested {};
  requested.samples = 4096;
  requested.freq = 22050;
  requested.format = AUDIO_S16;
  requested.channels = 2;
  requested.callback = &mixAudio;

  SDL_AudioSpec actual {};
  if(SDL_OpenAudio(&requested, &actual))
    Fail("Can't open audio");

  fprintf(stderr, "[audio] %d Hz %d ch\n", actual.freq, actual.channels);

  SDL_PauseAudio(0);
}

void destroy()
{
  SDL_PauseAudio(1);
  SDL_CloseAudio();
  SDL_Quit();
  fprintf(stderr, "OK\n");
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

    g_state.tick(getAudioTime());
    SDL_Delay(1);
  }

  destroy();
  return 0;
}

