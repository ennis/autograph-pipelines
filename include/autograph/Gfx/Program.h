#pragma once
#include <autograph/Config.h>
#include <autograph/Gfx/GLHandle.h>
#include <autograph/Gfx/ShaderStage.h>
#include <autograph/Gfx/StateGroup.h>
#include <string>

namespace ag {

//////////////////////////////////////////////
AG_GFX_API const char *getShaderStageName(gl::GLenum stage);

struct AG_GFX_API ProgramDeleter {
  static constexpr gl::GLenum objectType = gl::PROGRAM;
  void operator()(gl::GLuint obj) { gl::DeleteProgram(obj); }
};

struct AG_GFX_API ShaderDeleter {
  static constexpr gl::GLenum objectType = gl::SHADER;
  void operator()(gl::GLuint obj) { gl::DeleteShader(obj); }
};

//////////////////////////////////////////////
//
//
//
class AG_GFX_API ShaderObject {
public:
  gl::GLuint object() const { return obj_.get(); }
  static ShaderObject compile(gl::GLenum stage, const char *source);
  bool getCompileStatus();
  std::string getCompileLog();

private:
  GLHandle<ShaderDeleter> obj_;
};

//////////////////////////////////////////////
//
//
//
struct ShaderPPDefine {
  const char *define;
  const char *value;
};

//////////////////////////////////////////////
//
//
//
/**
 * @brief [brief description]
 * @details [long description]
 * @return [description]
 */
class AG_GFX_API ProgramObject {
public:
  ProgramObject() = default;
  ProgramObject(gl::GLuint obj) : obj_{obj} {}

  gl::GLuint object() const { return obj_.get(); }
  void attach(ShaderObject &sh);
  void link();
  bool getLinkStatus();
  std::string getLinkLog();
  void operator()(StateGroup &stateGroup) {
    stateGroup.program = obj_.get();
  }

  /**
   * @brief [brief description]
   * @details [long description]
   *
   * @param vs_src [description]
   * @param fs_src [description]
   * @param gs_src [description]
   * @param tcs_src [description]
   * @param tes_src [description]
   * @return [description]
   */
  static ProgramObject create(const char *vs_src, const char *fs_src,
                              const char *gs_src = nullptr,
                              const char *tcs_src = nullptr,
                              const char *tes_src = nullptr);

  /**
   * @brief [brief description]
   * @details [long description]
   *
   * @param cs_src [description]
   * @return [description]
   */
  static ProgramObject createCompute(const char *cs_src);

  // helper to create a program object from a single-source shader file
  /*static Program loadFromFile(
      const char* combined_source_file_path,
      ShaderStage stages, int numDefines, ShaderPPDefine* pDefines);*/

private:
  GLHandle<ProgramDeleter> obj_;
};
} // namespace ag
