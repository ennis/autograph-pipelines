#pragma once
#include <autograph/Config.h>
#include <autograph/Types.h>
#include <autograph/gl/GLHandle.h>
#include <autograph/gl/Texture.h>

namespace ag {
namespace gl {
//////////////////////////////////////////////
struct RenderbufferDeleter {
	static constexpr GLenum objectType = GL_RENDERBUFFER;
  void operator()(GLuint rb) { glDeleteRenderbuffers(1, &rb); }
};

class AG_API Renderbuffer {
public:
	struct Samples { int count; };
  Renderbuffer() = default;
  Renderbuffer(int w, int h, ImageFormat fmt, Samples samples = Samples{ 0 });

  auto object() { return obj_.get(); }
  auto width() const { return width_; }
  auto height() const { return height_; }
  auto format() const { return fmt_; }

private:
  ImageFormat fmt_;
  int width_;
  int height_;
  GLHandle<RenderbufferDeleter> obj_;
};

//////////////////////////////////////////////
struct FramebufferDeleter {
	static constexpr GLenum objectType = GL_FRAMEBUFFER;
  void operator()(GLuint fbo) { glDeleteFramebuffers(1, &fbo); }
};

//////////////////////////////////////////////
/**
 * @brief Framebuffer
 * @details Framebuffer
 */
class AG_API Framebuffer {
public:
  Framebuffer() {}
  Framebuffer(std::initializer_list<Texture *> colorTargets_,
              Texture *depthTarget_);

  Framebuffer(GLHandle<FramebufferDeleter> obj) : obj_{std::move(obj)} {}

  void setAttachement(GLenum attachement, GLuint tex);
  void setAttachement(GLenum attachement, Texture &tex);
  void setRenderbufferAttachement(GLenum attachement,
                                  Renderbuffer &renderbuffer);

  /*static Framebuffer create(std::initializer_list<Texture *> color_tex);
  static Framebuffer create(std::initializer_list<Texture *> color_tex,
                            Texture &depth_tex);*/
  static Framebuffer createDefault(int w, int h);

  bool ensureComplete();
  GLenum checkStatus();
  auto object() const { return obj_.get(); }
  auto width() const { return width_; }
  auto height() const { return height_; }

private:
  void checkDimensions(int w, int h);
  void ensureInitialized();
  void initialize();
  void bindFramebufferTextures();
  int width_{ 0 };
  int height_{ 0 };
  GLHandle<FramebufferDeleter> obj_;
};
}
}