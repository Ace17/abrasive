// Copyright (C) 2018 - Sebastien Alaiwan <sebastien.alaiwan@gmail.com>
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.

// SDL/OpenGL implementation for "Display" interface
#include "display.h"
#include "error.h"
#include "file.h"
#include "mesh.h"
#include "mat4.h"

#include <memory>
#include <map>
#include <vector>
#include <string>

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

static
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
    memcpy(dst, ((uint8_t*)surf->pixels) + (HEIGHT - 1 - row) * surf->pitch, ROW_SIZE);
    dst += ROW_SIZE;
  }

  CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, upsideDownBuffer.data()));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  CALL(glBindTexture(GL_TEXTURE_2D, 0));

  printf("[display] Loaded %s (%dx%d)\n", path, WIDTH, HEIGHT);

  return texture;
}

static
int compileShader(vector<uint8_t> code, int type)
{
  auto shaderId = glCreateShader(type);

  if(!shaderId)
    throw runtime_error("Can't create shader");

  printf("[display] compiling %s shader ... ", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
  auto srcPtr = (const char*)code.data();
  auto length = (GLint)code.size();
  CALL(glShaderSource(shaderId, 1, &srcPtr, &length));
  CALL(glCompileShader(shaderId));

  // Check compile result
  GLint Result;
  CALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result));

  if(!Result)
  {
    int logLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    vector<char> msg(logLength);
    glGetShaderInfoLog(shaderId, logLength, nullptr, msg.data());
    fprintf(stderr, "%s\n", msg.data());

    throw runtime_error("Can't compile shader");
  }

  printf("OK\n");

  return shaderId;
}

static
int linkShaders(vector<int> ids)
{
  // Link the program
  printf("[display] Linking shaders ... ");
  auto ProgramID = glCreateProgram();

  for(auto id : ids)
    glAttachShader(ProgramID, id);

  glLinkProgram(ProgramID);

  // Check the program
  GLint Result = GL_FALSE;
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);

  if(!Result)
  {
    int logLength;
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &logLength);
    vector<char> msg(logLength);
    glGetProgramInfoLog(ProgramID, logLength, nullptr, msg.data());
    fprintf(stderr, "%s\n", msg.data());

    throw runtime_error("Can't link shader");
  }

  printf("OK\n");

  return ProgramID;
}

struct OpenglDisplay : Display
{
  OpenglDisplay()
  {
    printf("[display] Starting\n");

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

    m_font = loadTexture("assets/font.bmp");

    GLuint vertexArray;
    CALL(glGenVertexArrays(1, &vertexArray));
    CALL(glBindVertexArray(vertexArray));

    CALL(glEnable(GL_DEPTH_TEST));
    CALL(glEnable(GL_CULL_FACE));
  }

  ~OpenglDisplay()
  {
    for(auto tex : m_textures)
      glDeleteTextures(1, &tex.second);

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    printf("[display] Shutdown\n");
  }

  void update() override
  {
    m_pulseLight *= 0.9;

    updateViewport();

    CALL(glClearColor(1, 0, 0, 1));
    CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    drawObjects();

    SDL_GL_SwapWindow(m_window);
    m_actors.clear();
  }

  void showText(const char* msg) override
  {
    printf("%s\n", msg);
  }

  void loadShader(int id, const char* path)
  {
    string basePath = path;
    auto VertexShaderCode = load<uint8_t>((basePath + "/vertex.glsl").c_str());
    auto FragmentShaderCode = load<uint8_t>((basePath + "/fragment.glsl").c_str());
    auto const vertexId = compileShader(VertexShaderCode, GL_VERTEX_SHADER);
    auto const fragmentId = compileShader(FragmentShaderCode, GL_FRAGMENT_SHADER);

    auto const progId = linkShaders({ vertexId, fragmentId });

    CALL(glDeleteShader(vertexId));
    CALL(glDeleteShader(fragmentId));

    grow(m_shaders, id);
    m_shaders[id] = progId;
  }

  void loadModel(int id, const char* path) override
  {
    Model model;

    model.mesh = loadMesh(path);
    auto& vertices = model.mesh.vertices;

    {
      model.diffuse = loadTexture("assets/textures/tex2.bmp");
    }

    {
      string basePath = path;
      model.lightmap = loadTexture((basePath + ".lightmap.bmp").c_str());
    }

    CALL(glGenBuffers(1, &model.vbo));
    CALL(glBindBuffer(GL_ARRAY_BUFFER, model.vbo));
    CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW));
    CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    grow(m_models, id);
    m_models[id] = model;
  }

  void pushActor(Actor const& actor) override
  {
    m_actors.push_back(actor);
  }

  void pulse() override
  {
    m_pulseLight = 1;
  }

  void setAmbientLight(float value) override
  {
    m_ambientLight = value;
  }

  void setLightPos(Vec3 pos) override
  {
    m_lightPos = pos;
  }

  void setCamera(Vec3 pos, Vec3 forward, Vec3 up) override
  {
    m_cameraPos = pos;
    m_cameraFwd = forward;
    m_cameraUp = up;
  }

private:
  float m_aspectRatio = 1.0f;
  double m_ambientLight = 0;
  double m_pulseLight = 0;
  Vec3 m_lightPos;
  Vec3 m_cameraPos, m_cameraFwd, m_cameraUp;
  std::vector<Actor> m_actors;
  SDL_Window* m_window;
  SDL_GLContext m_context;
  GLuint m_font;

  struct Model
  {
    Mesh mesh;
    GLuint vbo;
    GLuint diffuse;
    GLuint lightmap;
  };

  vector<Model> m_models;
  map<string, GLuint> m_textures;
  vector<GLuint> m_shaders;

  void updateViewport()
  {
    int width, height;
    SDL_GL_GetDrawableSize(m_window, &width, &height);
    m_aspectRatio = (float)width / height;
    CALL(glViewport(0, 0, width, height));
  }

  void drawObjects()
  {
    static const float fovy = (float)((60.0f / 180) * M_PI);
    static const float near_ = 0.1f;
    static const float far_ = 100.0f;
    const auto perspective = ::perspective(fovy, m_aspectRatio, near_, far_);

    auto const view = ::lookAt(m_cameraPos, m_cameraPos + m_cameraFwd, m_cameraUp);

    for(auto& actor : m_actors)
    {
      auto& model = m_models[actor.model];
      int program = m_shaders[actor.shader];
      CALL(glUseProgram(program));

      CALL(glUniform1f(getUniformIndex(program, "AmbientLight"), m_ambientLight + m_pulseLight));
      CALL(glUniform3f(getUniformIndex(program, "LightPos"),
                       m_lightPos.x,
                       m_lightPos.y,
                       m_lightPos.z));

      {
        auto const pos = ::translate(actor.pos);

        {
          auto mvp = getUniformIndex(program, "M");
          CALL(glUniformMatrix4fv(mvp, 1, GL_FALSE, &pos[0][0]));
        }

        auto mat = perspective * view * pos;

        auto mvp = getUniformIndex(program, "MVP");
        CALL(glUniformMatrix4fv(mvp, 1, GL_FALSE, &mat[0][0]));
      }

      CALL(glBindBuffer(GL_ARRAY_BUFFER, model.vbo));

      connectAttribute(0, 3, program, "vertexPos");
      connectAttribute(3, 3, program, "vertexNormal");
      connectAttribute(6, 2, program, "vertexUV");
      connectAttribute(8, 2, program, "vertexUV2");

      // Texture Unit 0: Diffuse
      CALL(glActiveTexture(GL_TEXTURE0));
      CALL(glBindTexture(GL_TEXTURE_2D, model.diffuse));
      CALL(glUniform1i(getUniformIndex(program, "DiffuseTex"), 0));

      // Texture Unit 1: Lightmap
      CALL(glActiveTexture(GL_TEXTURE1));
      CALL(glBindTexture(GL_TEXTURE_2D, model.lightmap));
      CALL(glUniform1i(getUniformIndex(program, "LightmapTex"), 1));

      CALL(glDrawArrays(GL_TRIANGLES, 0, model.mesh.vertices.size()));

      CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
      CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
  }

  GLuint loadTexture(const char* path)
  {
    auto i_tex = m_textures.find(path);

    if(i_tex == m_textures.end())
      m_textures[path] = ::loadTexture(path);

    return m_textures[path];
  }

  static void connectAttribute(int offset, int size, int program, const char* name)
  {
    auto const stride = sizeof(Vertex);
    auto const index = getAttributeIndex(program, name);
    auto pOffset = (const GLvoid*)(offset * sizeof(GLfloat));
    CALL(glEnableVertexAttribArray(index));
    CALL(glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pOffset));
  }

  static GLuint getAttributeIndex(int program, const char* name)
  {
    auto const index = glGetAttribLocation(program, name);

    if(index < 0)
      Fail("Unknown shader attribute: '%s'", name);

    return index;
  }

  static GLuint getUniformIndex(int program, const char* name)
  {
    auto const index = glGetUniformLocation(program, name);

    if(index < 0)
      Fail("Unknown shader attribute: '%s'", name);

    return index;
  }

  template<typename T>
  static void grow(vector<T>& v, int index)
  {
    if(index >= (int)v.size())
      v.resize(index + 1);
  }
};

unique_ptr<Display> createDisplay()
{
  return make_unique<OpenglDisplay>();
}

