#pragma once
#include <autograph/gl/Buffer.h>
#include <autograph/gl/DrawState.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Texture.h>
#include <autograph/support/Optional.h>
#include <autograph/support/SmallVector.h>
#include <sol/state.hpp>
#include <vector>

struct lua_State;

namespace ag {
namespace fx {

struct DrawStates {
  gl::RasterizerState rasterizerState;
  gl::DepthStencilState depthStencilState;

  // Blend states
  struct IndexedBlendState {
    int slot;
    gl::BlendState state;
  };
  SmallVector<IndexedBlendState, 8> blendStates;

  // viewports
  struct IndexedViewport {
    int slot;
    gl::Viewport v;
  };
  SmallVector<IndexedViewport, 8> viewports;
};

class Pass {
public:
private:
  // Dependencies
  SmallVector<Pass *, 4> dependencies;
};

class GeometryPass : public Pass {
public:
private:
};

class ScreenPass : public Pass {
public:
private:
};

/*class ComputePass : public Pass
{
public:
private:
  optional<int> group_size_x;
  optional<int> group_size_y;
  optional<int> group_size_z;
};*/

class Pipeline {
public:
  Pipeline();
  ~Pipeline();

  // set configuration options
  void setConfig(const char *name, int value);
  void setConfig(const char *name, float value);
  void setConfig(const char *name, bool value = true);

  // note that the same script won't be executed twice
  void loadScript(const char *path);
  Pass *getPass(const char *name);
  gl::Texture *getTexture(const char *name);
  // gl::Buffer* getBuffer(const char* name);

  // resource creation
  // gl::Texture& createTexture(

  gl::Texture &createTexture2D(const char *name, ImageFormat fmt, int w, int h,
                               int numMips);
  gl::Texture &createTexture3D(const char *name, ImageFormat fmt, int w, int h,
                               int d, int numMips);

private:
  //
  // allocated resources
  std::unordered_map<std::string, std::unique_ptr<gl::Texture>> textures;
  std::unordered_map<std::string, std::unique_ptr<gl::Buffer>> buffers;
  std::unordered_map<std::string, std::unique_ptr<Pass>> passes;
  // lua state for executing the scripts
  sol::state L;
};

class Effect {
public:
  class Pass {
  public:
    Pass(Effect &refEffect_, const char *name_)
        : effect{refEffect_}, name{name_} {}

  private:
    Effect &effect;
    std::string name;
    // state to set: opengl programs, render states, textures
  };

  // Create an empty effect with no passes
  Effect() = default;
  // set a configuration option
  void setConfig(const char *name, int value);
  void setConfig(const char *name, float value);
  void setConfig(const char *name, bool value = true);
  // load an effect file from a path
  void loadFromFile(const char *path);
  // this will execute the lua script and generate the passes
  void compile();
  // find a pass by name
  Pass *findPass(const char *name);

private:
  // source path
  std::string path;
  // lua script source
  std::string scriptSource;
  // allocated textures
  std::vector<gl::Texture> textures;
  // framebuffers
  std::vector<gl::Framebuffer> framebuffers;
  // loaded passes
};

//
// drawScreenQuad(
//    pass,
//    vertexBuffer(...),
//    indexBuffer(...),
//    texture(id, resource))

// User loads file
// Set configuration options
// Get passes: this will trigger the compilation
// Resetting a configuration option will invalidate the passes
}
}