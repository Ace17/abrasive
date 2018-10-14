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

  void tick(double clock) override
  {
    updateState(clock);
    pushActors();
  }

  void updateState(double now)
  {
    while(curr + 1 < (int)timeline.size() && now >= timeline[curr + 1].clockTime)
    {
      curr++;
      m_display->showText(timeline[curr].text);
    }

    Vec3 lightPos;
    lightPos.x = cos(now * 2.0 * M_PI * 0.25) * 3.0;
    lightPos.y = sin(now * 2.0 * M_PI * 0.25) * 3.0;
    lightPos.z = 0.0;
    m_display->setAmbientLight(0);
    m_display->setLightPos(lightPos);
    m_display->setCamera(m_pos, { 0, 1, 0 }, { 0, 0, 1 });
  }

  void pushActors()
  {
    Actor actor;
    actor.model = MODEL_ROOM;
    actor.shader = SHADER_BASIC;
    m_display->pushActor(actor);
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

