#pragma once
#include <array>
#include <autograph/Core/Types.h>
#include <autograph/Gfx/Buffer.h>
#include <autograph/Gfx/Fence.h>
#include <autograph/Gfx/Framebuffer.h>
#include <autograph/Core/Support/Debug.h>

namespace ag {

//----------------------------------------------------------
// Frame capture macros
template <typename... Args>
void frameTrace(const char *function, const char *message, Args &&... args) {
  std::string str{function};
  str.append(": ");
  str.append(fmt::format(message, std::forward<Args>(args)...));
  rawLogMessage(LogLevel::Debug, str.c_str());
}

#ifdef AG_FRAME_TRACE_OUTPUT
#define AG_FRAME_TRACE(...)                                                    \
  do {                                                                         \
    if (getGfxContext().isFrameCaptureRequested())                             \
      ag::frameTrace(__func__, __VA_ARGS__);                                   \
  } while (0);
#else
#define AG_FRAME_TRACE(...)
#endif

//----------------------------------------------------------
// Context config
struct GfxConfig {
  int maxFramesInFlight = 0;
  int defaultUploadBufferSize = 0;
};

struct GLImplementationLimits {
  int max_vertex_attributes = 16;
  int max_vertex_buffers = 16;
  int max_texture_units = 16;
  int max_draw_buffers = 16;
  int max_3d_texture_size_w;
  int max_3d_texture_size_h;
  int max_3d_texture_size_d;
  int max_combined_texture_image_units = 16;
  int max_combined_uniform_blocks = 16;
  int max_combined_shader_storage_blocks = 16;
  int max_compute_texture_image_units = 16;
  int max_compute_uniform_blocks = 16;
  int max_compute_shader_storage_blocks = 16;
  int max_compute_work_group_invocations;
  int max_compute_work_group_count;
  int max_compute_work_group_size;
  int uniform_buffer_alignment = 256;
  int default_compute_local_size_x;
  int default_compute_local_size_y;
  int default_compute_local_size_z;
};

//----------------------------------------------------------
// Context
class AG_GFX_API GfxContext {
public:
  GfxContext(const GfxConfig &cfg);
  ~GfxContext();

  void setFrameCapture(int64_t targetFrameIndex) {
    nextFrameCapture = targetFrameIndex;
  }
  void setNextFrameCapture() { nextFrameCapture = getCurrentFrameIndex() + 1; }

  bool isFrameCaptureRequested() {
    return nextFrameCapture == getCurrentFrameIndex();
  }

  // disable copy and move
  const GLImplementationLimits &getGLImplementationLimits();
  void resizeRenderWindow(int w, int h);
  ivec2 getRenderWindowSize();
  const GfxConfig &getConfig() const { return cfg; }

  int64_t getCurrentFrameIndex();
  void beginFrame();
  void endFrame();
  Framebuffer& getDefaultFramebuffer() { return screenFbo; }

private:
  GLImplementationLimits implLimits;
  GfxConfig cfg;
  Fence frameFence;
  int64_t frameIndex;
  int width;
  int height;
  Framebuffer screenFbo;
  // Graphics frame capture
  int64_t nextFrameCapture = -1;
};

// get/set global graphics context
// used by graphics resources (Textures, Buffers, etc.), instead of explicitly
// passing the context every time as a constructor parameter
// Can be used to get the default framebuffer
// This might disappear in the future
AG_GFX_API void setGfxContext(GfxContext *ctx);
AG_GFX_API GfxContext &getGfxContext();
} // namespace ag