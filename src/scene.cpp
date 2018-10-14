#include "scene.h"

#include "error.h"
#include <map>
#include <string>

using namespace std;

static map<std::string, SceneCreationFunc*> g_scenes;

int registerScene(std::string name, SceneCreationFunc* func)
{
  if(g_scenes.find(name) != g_scenes.end())
    Fail("a scene named '%s' is already registered", name);

  g_scenes[name] = func;
  return 0;
}

unique_ptr<Scene> createScene(std::string name, Display* display)
{
  if(g_scenes.find(name) == g_scenes.end())
    Fail("unknown scene '%s'", name);

  return unique_ptr<Scene>(g_scenes[name] (display));
}

