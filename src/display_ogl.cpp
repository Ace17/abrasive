#include "display.h"
#include "error.h"
#include "mesh.h"

#include <memory>
#include <vector>

#include "SDL.h"

#define GL_GLEXT_PROTOTYPES 1
#include "GL/gl.h"

using namespace std;

static void checkGl(const char* caption, const char* file, int line)
{
#ifndef NDEBUG
  auto errorCode = glGetError();

  if(errorCode != GL_NO_ERROR)
    Fail("[%s:%d] %s returned %d\n", file, line, caption, errorCode);

#endif
}

#define CALL(a) \
  do { a; checkGl(# a, __FILE__, __LINE__); } while(0)

GLuint loadTexture(const char* path)
{
  auto surf = shared_ptr<SDL_Surface>(SDL_LoadBMP(path), &SDL_FreeSurface);

  if(!surf)
    Fail("Can't load texture '%s'", path);

  GLuint texture;
  CALL(glGenTextures(1, &texture));
  CALL(glBindTexture(GL_TEXTURE_2D, texture));

  auto const WIDTH = surf->w;
  auto const HEIGHT = surf->h;

  // vertically flip the picture so the bottom-left corresponds to UV (0;0).
  // (This is ironic, as SDL just did the reverse operation
  // after loading the BMP, which is stored upside down).
  vector<uint8_t> upsideDownBuffer(WIDTH* HEIGHT * 4);
  auto dst = upsideDownBuffer.data();

  for(int row = 0; row < HEIGHT; ++row)
  {
    auto const ROW_SIZE = WIDTH * 4;
    memcpy(dst, ((uint8_t*)surf->pixels) + row * surf->pitch, ROW_SIZE);
    dst += ROW_SIZE;
  }

  CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, upsideDownBuffer.data()));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
  CALL(glBindTexture(GL_TEXTURE_2D, 0));

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

    CALL(glClearColor(m_ambientLight, m_ambientLight, m_ambientLight, 1));
    CALL(glClear(GL_COLOR_BUFFER_BIT));
    SDL_GL_SwapWindow(m_window);
  }

  void showText(const char* msg) override
  {
    printf("%s\n", msg);
  }

  void loadModel(int id, const char* path) override
  {
    if(id >= m_meshes.size())
      m_meshes.resize(id + 1);

    m_meshes[id] = loadMesh(path);
  }

  void drawModel(int id, Vec3 pos)
  {
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
  vector<Mesh> m_meshes;
};

unique_ptr<Display> createDisplay()
{
  return make_unique<OpenglDisplay>();
}

