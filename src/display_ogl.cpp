#include "display.h"
#include "error.h"
#include <memory>

#include "SDL.h"

#define GL_GLEXT_PROTOTYPES 1
#include "GL/gl.h"

using namespace std;

GLuint loadTexture(const char* path)
{
  auto surf = shared_ptr<SDL_Surface>(SDL_LoadBMP(path), &SDL_FreeSurface);
  if(!surf)
    Fail("Can't load texture '%s'", path);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);

  return 0;
}

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

    m_font = loadTexture("assets/font.bmp");

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
    m_ambientLight *= 0.97;

    glClearColor(m_ambientLight, m_ambientLight, m_ambientLight, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(m_window);
  }

  void showText(const char* msg) override
  {
    printf("%s\n", msg);
  }

  void pulse() override
  {
    m_ambientLight = 0.5;
  }

private:
  double m_ambientLight = 0;
  SDL_Window* m_window;
  SDL_GLContext m_context;
  GLuint m_font;
};

unique_ptr<Display> createDisplay()
{
  return make_unique<OpenglDisplay>();
}

