#include "audio.h"
#include <vector>
#include <fstream>
#include <atomic>
#include "SDL.h"

using namespace std;

void Fail(const char* fmt, ...)
{
  va_list va;
  va_start(va, fmt);
  char buffer[4096] {};
  vsnprintf(buffer, sizeof buffer - 1, fmt, va);
  fprintf(stderr, "Fatal: %s\n", buffer);
  exit(1);
}

struct SdlAudio : Audio
{
  SdlAudio()
  {
    m_music = load("assets/music.pcm");

    if(SDL_InitSubSystem(SDL_INIT_AUDIO))
      Fail("Can't init audio");

    SDL_AudioSpec requested {};
    requested.samples = 4096;
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

  ~SdlAudio()
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
    fp.read((char*)buf.data(), buf.size() * sizeof(int16_t));

    return buf;
  }

  static void staticMixAudio(void* userParam, Uint8* buffer, int size)
  {
    auto pThis = (SdlAudio*)userParam;
    memset(buffer, 0, size);
    pThis->mixSamples((short*)buffer, size / sizeof(short));
  }

  void mixSamples(short* samples, int count)
  {
    auto const deltaTime = count / CHANNELS;

    while(count > 0)
    {
      auto remaining = (int)m_music.size() - m_musicPos;
      auto readSize = std::min(count, remaining);
      memcpy(samples, m_music.data() + m_musicPos, readSize * sizeof(short));
      m_musicPos += readSize;

      // loop point is halfway
      if(m_musicPos >= m_music.size())
        m_musicPos = m_music.size() / 2;

      count -= readSize;
    }

    m_timeInSamples += deltaTime;
  }

  static auto const CHANNELS = 2;
  static auto const SAMPLERATE = 22050;
};

unique_ptr<Audio> createAudio()
{
  return make_unique<SdlAudio>();
}

