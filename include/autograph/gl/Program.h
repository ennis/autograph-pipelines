#pragma once
#include <autograph/Config.h>
#include <autograph/gl/ShaderStage.h>
#include <autograph/gl/GLHandle.h>
#include <string>

namespace ag {
namespace gl {

//////////////////////////////////////////////
AG_API const char *getShaderStageName(GLenum stage);

struct ProgramDeleter {
  void operator()(GLuint obj) { glDeleteProgram(obj); }
};

struct ShaderDeleter {
  void operator()(GLuint obj) { glDeleteShader(obj); }
};

//////////////////////////////////////////////
//
//
//
class AG_API Shader {
public:
  GLuint object() const { return obj_.get(); }
  static Shader compile(GLenum stage, const char *source);
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
class AG_API Program {
public:
  Program() = default;
  Program(GLuint obj) : obj_{obj} {}

  GLuint object() const { return obj_.get(); }
  void attach(Shader &sh);
  void link();
  bool getLinkStatus();
  std::string getLinkLog();

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
  static Program create(const char *vs_src, const char *fs_src,
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
  static Program createCompute(const char *cs_src);

  // helper to create a program object from a single-source shader file
  /*static Program loadFromFile(
      const char* combined_source_file_path,
      ShaderStage stages, int numDefines, ShaderPPDefine* pDefines);*/

private:
  GLHandle<ProgramDeleter> obj_;
};
}
}