#include "display.h"
#include "error.h"
#include <memory>

#include "SDL.h"

#define GL_GLEXT_PROTOTYPES 1
#include "GL/gl.h"

using namespace std;

struct OpenglDisplay : Display
{
  OpenglDisplay()
  {
    if(SDL_InitSubSystem(SDL_INIT_VIDEO))
      Fail("Can't init SDL");

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE | SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_window = SDL_CreateWindow(
        "Abrasive",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1920, 1080,
        SDL_WINDOW_OPENGL
        );

    if(!m_window)
      Fail("Can't create SDL window");

    m_context = SDL_GL_CreateContext(m_window);

    if(!m_context)
      Fail("Can't create OpenGL context");

    // V-sync
    SDL_GL_SetSwapInterval(1);
  }

  ~OpenglDisplay()
  {
    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }

  void update() override
  {
    m_ambientLight *= 0.99;
    glClearColor(m_ambientLight, m_ambientLight, m_ambientLight, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(m_window);
  }

  void showText(const char* msg) override
  {
    printf("%s\n", msg);
    m_ambientLight = 0.5;
  }

private:
  double m_ambientLight = 0;
  SDL_Window* m_window;
  SDL_GLContext m_context;
};

unique_ptr<Display> createDisplay()
{
  return make_unique<OpenglDisplay>();
}

