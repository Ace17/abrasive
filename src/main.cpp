#include <cstdio>
#include <cstdint>
#include <vector>
#include <memory>
#include <fstream>
#include <atomic>

using namespace std;

auto const BPM = 72.0;

struct Event
{
  double clockTime; // in beats
  const char* text = "";
};

const vector<Event> timeline =
{
  { 0.00, "0" },
  { 1.00, "1" },
  { 2.00, "2" },
  { 3.00, "3" },
  { 4.00, "4" },
  { 5.00, "5" },
  { 6.00, "6" },
  { 7.00, "7" },
  { 8.00, "8" },
  { 16.00, "16" },
  { 24.00, "24" },
  { 32.00, "32" },
} ;

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

    while(curr + 1 < (int)timeline.size() && now >= timeline[curr+1].clockTime)
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

struct Audio
{
  Audio()
  {
    m_music = load("assets/music.pcm");

    if(SDL_InitSubSystem(SDL_INIT_AUDIO))
      Fail("Can't init audio");

    SDL_AudioSpec requested {};
    requested.samples = 128;
    requested.freq = SAMPLERATE;
    requested.format = AUDIO_S16;
    requested.channels = 2;
    requested.callback = &staticMixAudio;
    requested.userdata = this;

    SDL_AudioSpec actual {};
    if(SDL_OpenAudio(&requested, &actual))
      Fail("Can't open audio");

    fprintf(stderr, "[audio] %d Hz %d ch %d samples (%.2f ms)\n", actual.freq, actual.channels, actual.samples, actual.samples * 1000.0 / double(actual.freq));

    m_timeInSamples = -actual.samples;
    SDL_PauseAudio(0);
  }

  ~Audio()
  {
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
  }

  double getTime()
  {
    return m_timeInSamples / double(SAMPLERATE);
  }

private:
  vector<int16_t> m_music;
  int m_musicPos = 0;

  std::atomic<int64_t> m_timeInSamples; // incremented at each audio callback

  static
  vector<int16_t> load(const char* path)
  {
    auto fp = ifstream(path, ios::binary);
    if(!fp.is_open())
      Fail("Can't open music file: '%s'", path);

    fp.seekg(0, ios::end);
    auto size = fp.tellg();
    fp.seekg(0, ios::beg);

    vector<int16_t> buf;
    buf.resize(size / sizeof(int16_t));
    fp.read((char*)buf.data(), buf.size());

    return buf;
  }

  static void staticMixAudio(void* userParam, Uint8* buffer, int size)
  {
    auto pThis = (Audio*)userParam;
    memset(buffer, 0, size);
    pThis->mixSamples((short*)buffer, size/sizeof(short));
  }

  void mixSamples(short* samples, int count)
  {
    auto remaining = (int)m_music.size() - m_musicPos;
    auto readSize = std::min(count, remaining);
    memcpy(samples, m_music.data() + m_musicPos, readSize * sizeof(short));
    m_musicPos += readSize;

    m_timeInSamples += count / CHANNELS;
  }

  static auto const CHANNELS = 2;
  static auto const SAMPLERATE = 22050;
};

unique_ptr<Audio> g_Audio;

void init()
{
  SDL_InitSubSystem(SDL_INIT_EVENTS);
  g_Audio = make_unique<Audio>();
}

void destroy()
{
  g_Audio.reset();
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

    g_state.tick(g_Audio->getTime());
    SDL_Delay(1);
  }

  destroy();
  return 0;
}

