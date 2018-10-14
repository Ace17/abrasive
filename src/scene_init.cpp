#include "scene.h"
#include "display.h"
#include "vec3.h"

#include <vector>
#include <cmath>

using namespace std;

namespace
{
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
  { 16.00, "16 - Bass starts" },
  { 18.00, "18 - " },
  { 20.00, "20 - " },
  { 22.00, "22 - " },
  { 24.00, "24 - " },
  { 26.00, "26 - " },
  { 28.00, "28 - " },
  { 30.00, "30 - " },
  { 32.00, "32 - Drumkit is rolling" },
  { 48.00, "48 - Saw FX" },
  { 64.00, "64 - Breakdown Sawtooth porta" },
  { 96.00, "96 - End" },
};

struct InitScene : Scene
{
  InitScene(Display* display) :
    m_display(display)
  {
  }

  bool tick(double clock) override
  {
    updateState(clock);
    pushActors();
    return clock < 32.0;
  }

  void updateState(double now)
  {
    while(now >= 32.0)
      now -= 32.0;

    while(curr + 1 < (int)timeline.size() && now >= timeline[curr + 1].clockTime)
    {
      curr++;
      m_display->showText(timeline[curr].text);

      if(now >= 16)
        m_display->pulse();
    }

    if(now < 16)
    {
      Vec3 lightPos;
      lightPos.x = cos(now * 2.0 * M_PI * 0.25) * 3.0;
      lightPos.y = sin(now * 2.0 * M_PI * 0.25) * 3.0;
      lightPos.z = 0.0;
      m_display->setAmbientLight(0);
      m_display->setLightPos(lightPos);
      m_display->setCamera(m_pos, { 0, 1, 0 }, { 0, 0, 1 });
    }
    else
    {
      m_pos = m_pos + Vec3 { 0, 0.06, 0 };
      m_display->setLightPos(m_pos + Vec3 { 0, 1, 0 });
      auto t = now - 16;
      Vec3 up;
      up.x = sin(t * 0.1);
      up.y = 0;
      up.z = cos(t * 0.1);

      m_display->setAmbientLight((now - 16) * 0.01);
      m_display->setCamera(m_pos, { 0, 1, 0 }, up);
    }

    if(0)
      m_display->setAmbientLight(0.5);
  }

  void pushActors()
  {
    if(0)
    {
      Actor actor;
      actor.pos = { 3, 0, 0 };
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

    m_display->update();
  }

private:
  int curr = -1;

  Vec3 m_pos = { 0, -5, 0 };
  Display* const m_display;
};

Scene* create(Display* display)
{
  return new InitScene(display);
}

static int registered = registerScene("init", &create);
}

