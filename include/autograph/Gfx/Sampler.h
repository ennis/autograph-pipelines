#pragma once
#include "GLHandle.h"
#include <autograph/Config.h>
#include <autograph/Core/Support/HashCombine.h>
#include <memory>
#include <unordered_map>

namespace ag {

struct AG_GFX_API SamplerDeleter {
  static constexpr gl::GLenum objectType = gl::SAMPLER;
  void operator()(gl::GLuint sam_obj) { gl::DeleteSamplers(1, &sam_obj); }
};

/// OpenGL sampler object
/// Sampler objects are cached:
/// Use Sampler::get(Desc) to get a cached sampler object with the specified
/// parameters instead of creating a new one every time
class AG_GFX_API Sampler {
public:
  /// Sampler parameters
  /// TODO rename to 'Parameters'?
  struct Desc {
    /// U-coordinate repeat mode (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
    gl::GLenum addrU = gl::REPEAT;
    /// V-coordinate repeat mode (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
    gl::GLenum addrV = gl::REPEAT;
    /// W-coordinate repeat mode (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
    gl::GLenum addrW = gl::REPEAT;
    /// Minification filtering mode (see OpenGL docs for allowed options)
    gl::GLenum minFilter = gl::NEAREST;
    /// Magnification filtering mode (see OpenGL docs for allowed options)
    gl::GLenum magFilter = gl::NEAREST;
    /// Border color for use with the GL_CLAMP_TO_BORDER repeat mode
    std::array<float, 4> borderColor = {
        {0.0f, 0.0f, 0.0f,
         0.0f}}; // vec4 is not constexpr, so use an array instead

    /// Comparison operator
    constexpr bool operator==(const Desc &rhs) const {
      return addrU == rhs.addrU && addrV == rhs.addrV && addrW == rhs.addrW &&
             minFilter == rhs.minFilter && magFilter == rhs.magFilter &&
             borderColor == rhs.borderColor;
    }

    /// Hasher
    struct Hash {
      constexpr std::size_t operator()(Sampler::Desc const &s) const {
        std::size_t res = 0;
        hashCombine(res, s.addrU);
        hashCombine(res, s.addrV);
        hashCombine(res, s.addrW);
        hashCombine(res, s.minFilter);
        hashCombine(res, s.magFilter);
        hashCombine(res, s.borderColor[0]);
        hashCombine(res, s.borderColor[1]);
        hashCombine(res, s.borderColor[2]);
        hashCombine(res, s.borderColor[3]);
        return res;
      }
    };
  };

  /// Description for a sampler with all repeat modes set to REPEAT, and
  /// filtering modes set to NEAREST
  static constexpr Desc NearestRepeatDesc =
      Desc{gl::REPEAT,  gl::REPEAT,  gl::REPEAT,
           gl::NEAREST, gl::NEAREST, {{0.0f, 0.0f, 0.0f, 0.0f}}};

  /// Description for a sampler with all repeat modes set to REPEAT, and
  /// filtering modes set to LINEAR
  static constexpr Desc LinearRepeatDesc =
      Desc{gl::REPEAT, gl::REPEAT, gl::REPEAT,
           gl::LINEAR, gl::LINEAR, {{0.0f, 0.0f, 0.0f, 0.0f}}};

  /// Default constructor: no sampler allocated (cannot bind to pipeline)
  Sampler() {}
  /// Create a sampler with the specified parameters
  Sampler(const Desc &desc) : desc_{desc} { init(); }

  gl::GLuint object() const { return obj_.get(); }

  /*void setWrapModeU(gl::GLenum mode);
  void setWrapModeV(gl::GLenum mode);
  void setWrapModeW(gl::GLenum mode);
  void setTextureMinFilter(gl::GLenum filter);
  void setTextureMagFilter(gl::GLenum filter);
  void setBorderColor(float r, float g, float b, float a);
  void setBorderColor(const vec4 &rgba);*/

  static const Sampler &linearRepeat();
  static const Sampler &nearestRepeat();
  /// Get a cached sampler object matching the specified parameters
  static const Sampler &get(const Desc &desc);

private:
  Desc desc_;
  GLHandle<SamplerDeleter> obj_;
  void init();

  static std::unordered_map<Desc, std::unique_ptr<Sampler>, Sampler::Desc::Hash>
      samplerCache;
};

} // namespace ag
