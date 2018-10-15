#include "audio.h"
#include "error.h"
#include "file.h"

#include <vector>
#include <atomic>
#include "SDL.h"

using namespace std;

struct SdlAudio : Audio
{
  SdlAudio(const char* musicPath)
  {
    m_music = load<int16_t>(musicPath);

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

    printf("[audio] %d Hz %d ch %d samples (%.2f ms)\n", actual.freq, actual.channels, actual.samples, actual.samples * 1000.0 / double(actual.freq));

    m_timeInSamples = -actual.samples;
    m_timeInTicks = 0;
    SDL_PauseAudio(0);
  }

  ~SdlAudio()
  {
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    printf("[audio] Shutdown\n");
  }

  double getTime()
  {
    return m_timeInSamples / double(SAMPLERATE) + (SDL_GetTicks() - m_timeInTicks) / 1000.0;
  }

private:
  vector<int16_t> m_music;
  int m_musicPos = 0;

  std::atomic<int64_t> m_timeInSamples; // incremented at each audio callback
  std::atomic<int64_t> m_timeInTicks; // set at each audio callback

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
      if(m_musicPos >= (int)m_music.size())
        m_musicPos = m_music.size() / 2;

      samples += readSize;
      count -= readSize;
    }

    m_timeInSamples += deltaTime;
    m_timeInTicks = SDL_GetTicks();
  }

  static auto const CHANNELS = 2;
  static auto const SAMPLERATE = 22050;
};

unique_ptr<Audio> createAudio(const char* musicPath)
{
  return make_unique<SdlAudio>(musicPath);
}

