#include <cstdio>
#include <cstdint>
#include <vector>
#include <memory>
#include <fstream>

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

struct Audio
{
  Audio()
  {
    m_music = load("assets/music.pcm");

    if(SDL_InitSubSystem(SDL_INIT_AUDIO))
      Fail("Can't init audio");

    SDL_AudioSpec requested {};
    requested.samples = 8192;
    requested.freq = 22050;
    requested.format = AUDIO_S16;
    requested.channels = 2;
    requested.callback = &staticMixAudio;
    requested.userdata = this;

    SDL_AudioSpec actual {};
    if(SDL_OpenAudio(&requested, &actual))
      Fail("Can't open audio");

    fprintf(stderr, "[audio] %d Hz %d ch %d samples (%.2f ms)\n", actual.freq, actual.channels, actual.samples, actual.samples * 1000.0 / double(actual.freq));

    SDL_PauseAudio(0);
  }

  ~Audio()
  {
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
  }

private:
  vector<int16_t> m_music;
  int m_musicReadPos = 0;

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
    pThis->mixAudio(buffer, size);
  }

  void mixAudio(uint8_t* dst, int size)
  {
    memset(dst, 0, size);
    g_totalSamples += size / 4;

    auto remaining = (int)m_music.size() - m_musicReadPos;
    auto readSize = std::min(size, remaining);
    memcpy(dst, (uint8_t*)m_music.data() + m_musicReadPos, readSize);
    m_musicReadPos += readSize;
  }
};

int64_t getAudioTime()
{
  return g_totalSamples * 1000.0 / 22050.0;
}

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

    g_state.tick(getAudioTime());
    SDL_Delay(1);
  }

  destroy();
  return 0;
}

