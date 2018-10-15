// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// =============================================================================
// A B R A S I V E
//
// by Sebastien Alaiwan
//
// =============================================================================

// Main app
#include <cstdio>
#include <cstdint>
#include <memory>

#include "audio.h"
#include "display.h"
#include "scene.h"

using namespace std;

template<typename T, size_t N>
size_t ArrayLength(T const (&)[N]) { return N; }

struct SceneInstance
{
  const char* name;
  double duration; // in beats
};

static const SceneInstance g_scenePlaylist[] =
{
  { "init", 16.0 },
  { "bass", 16.0 },
};

// in beats
double getClock(double seconds)
{
  static auto const BPM = 72.5;
  return seconds * (BPM / 60);
}

struct State
{
  State(int sceneIndex) :
    m_sceneIndex(sceneIndex),
    m_display(createDisplay()),
    m_audio(createAudio("assets/music.pcm"))
  {
    m_display->loadShader(SHADER_BASIC, "assets/shaders/basic");
    m_display->loadModel(MODEL_BOX, "assets/meshes/box.mesh");
    m_display->loadModel(MODEL_ROOM, "assets/meshes/room.mesh");
  }

  void tick()
  {
    auto clock = getClock(m_audio->getTime());

    if(!m_scene)
    {
      auto instance = g_scenePlaylist[m_sceneIndex];
      m_scene = createScene(instance.name, m_display.get());
      m_sceneStartTime = clock;
      printf("Scene: %s\n", instance.name);
    }

    m_scene->tick(clock - m_sceneStartTime);

    if(0)
    {
      Actor actor;
      actor.pos = { 3, 0, 0 };
      actor.model = MODEL_BOX;
      actor.shader = SHADER_BASIC;
      m_display->pushActor(actor);
      m_display->setAmbientLight(0.5);
    }

    m_display->update();

    if(clock - m_sceneStartTime >= g_scenePlaylist[m_sceneIndex].duration)
    {
      m_sceneIndex = (m_sceneIndex + 1) % ArrayLength(g_scenePlaylist);
      m_scene.reset();
    }
  }

private:
  double m_sceneStartTime = 0;
  int m_sceneIndex = 0;
  unique_ptr<Scene> m_scene;
  unique_ptr<Display> m_display;
  unique_ptr<Audio> m_audio;
};

///////////////////////////////////////////////////////////////////////////////
// Here begins the SDL-dependent part

#include "SDL.h"

static unique_ptr<State> g_state;

void init(int sceneIndex)
{
  SDL_InitSubSystem(SDL_INIT_EVENTS);
  g_state = make_unique<State>(sceneIndex);
}

void destroy()
{
  g_state.reset();
  SDL_QuitSubSystem(SDL_INIT_EVENTS);
  fprintf(stderr, "OK\n");
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  int sceneIndex = 0;

  if(argc >= 2)
    sceneIndex = atoi(argv[1]);

  init(sceneIndex);

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

