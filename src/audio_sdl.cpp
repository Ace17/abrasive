// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// SDL-implementation of audio
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
    if(SDL_InitSubSystem(SDL_INIT_AUDIO))
      Fail("Can't init audio");

    SDL_AudioSpec requested {};
    requested.samples = 4096;
    requested.freq = 48000;
    requested.format = AUDIO_S16;
    requested.channels = 2;
    requested.callback = &staticMixAudio;
    requested.userdata = this;

    if(SDL_OpenAudio(&requested, &m_audioSpec))
      Fail("Can't open audio");

    printf("[audio] %d Hz %d ch %d samples (%.2f ms)\n", m_audioSpec.freq, m_audioSpec.channels, m_audioSpec.samples, m_audioSpec.samples * 1000.0 / double(m_audioSpec.freq));

    m_music = loadMusic(musicPath);

    m_timeInSamples = 0;
    m_timeInTicks = SDL_GetTicks();
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
    return m_timeInSamples / double(m_audioSpec.freq) + (SDL_GetTicks() - m_timeInTicks) / 1000.0;
  }

  vector<uint8_t> loadMusic(const char* musicPath)
  {
    SDL_AudioCVT cvt {};

    static auto const MUSIC_CHANNELS = 2;
    static auto const MUSIC_SAMPLERATE = 22050;

    if(SDL_BuildAudioCVT(&cvt,
                         AUDIO_S16, MUSIC_CHANNELS, MUSIC_SAMPLERATE,
                         m_audioSpec.format, m_audioSpec.channels, m_audioSpec.freq) < 0)
      Fail("Can't build audio converter");

    auto music = load<uint8_t>(musicPath);
    auto origSize = music.size();

    if(0)
    {
      // synchro debug: generate click track
      auto const BPM = 72.5;
      auto p = (short*)music.data();

      for(int i = 0; i < (int)music.size() / 4; ++i)
      {
        auto tSecs = i / double(MUSIC_SAMPLERATE);
        auto tBeat = tSecs * (BPM / 60.0);

        if((tBeat - floor(tBeat)) < 0.2)
        {
          auto s = int(sin(tSecs * 2 * M_PI * 880.0) * 32767.0 * 0.1);
          *p++ = s;
          *p++ = s;
        }
        else
        {
          *p++ = 0;
          *p++ = 0;
        }
      }
    }

    music.resize(origSize * cvt.len_mult);
    cvt.buf = music.data();
    cvt.len = origSize;

    if(SDL_ConvertAudio(&cvt) < 0)
      Fail("Can't convert audio");

    music.resize(cvt.len_cvt);
    music.shrink_to_fit();

    return music;
  }

private:
  SDL_AudioSpec m_audioSpec;
  vector<uint8_t> m_music;
  int m_musicPos = 0;

  std::atomic<int64_t> m_timeInSamples; // incremented at each audio callback
  std::atomic<int64_t> m_timeInTicks; // set at each audio callback

  static void staticMixAudio(void* userParam, Uint8* buffer, int size)
  {
    auto pThis = (SdlAudio*)userParam;
    memset(buffer, 0, size);
    pThis->mixSamples((uint8_t*)buffer, size);
  }

  void mixSamples(uint8_t* dstBuf, int dstLen)
  {
    auto const bytesPerSample = (m_audioSpec.format & 0xFF) / 8;
    auto const deltaTime = dstLen / (bytesPerSample * m_audioSpec.channels);

    while(dstLen > 0)
    {
      auto remaining = (int)m_music.size() - m_musicPos;
      auto readSize = std::min(dstLen, remaining);
      memcpy(dstBuf, m_music.data() + m_musicPos, readSize);
      m_musicPos += readSize;

      // loop point is halfway
      if(m_musicPos >= (int)m_music.size())
        m_musicPos = m_music.size() / 2;

      dstBuf += readSize;
      dstLen -= readSize;
    }

    m_timeInSamples += deltaTime;
    m_timeInTicks = SDL_GetTicks();
  }
};

unique_ptr<Audio> createAudio(const char* musicPath)
{
  return make_unique<SdlAudio>(musicPath);
}

