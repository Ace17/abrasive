#include "scene.h"
#include "display.h"
#include "vec3.h"

#include <vector>
#include <cmath>

using namespace std;

namespace
{
static double frac(double val)
{
  return val - floor(val);
}

struct BassScene : Scene
{
  BassScene(Display* display) :
    m_display(display)
  {
  }

  void tick(double clock) override
  {
    if(frac(clock * 0.5) < 0.01)
      m_display->pulse();

    updateState(clock);
    pushActors();
  }

  void updateState(double now)
  {
    Vec3 pos = { 0, -5 + float(now * 2.0), 0 };
    auto t = now;
    Vec3 up;
    up.x = sin(t * 0.1);
    up.y = 0;
    up.z = cos(t * 0.1);

    m_display->setLightPos(pos + Vec3 { 0, float(min(now, 10.0)), 0 });

    m_display->setAmbientLight(now * 0.01);
    m_display->setCamera(pos, { 0, 1, 0 }, up);
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

  Display* const m_display;
};

Scene* create(Display* display)
{
  return new BassScene(display);
}

static int registered = registerScene("bass", &create);
}

