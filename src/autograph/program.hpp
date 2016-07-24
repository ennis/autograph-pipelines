#pragma once
#include "gl_handle.hpp"
#include "shader_stage.hpp"
#include <ostream>
#include <experimental/filesystem>
#include <tuple>

namespace ag {

// GLuint compile_shader(GLenum stage, const char *source, std::ostream
// &infoLog);
// bool link_program(GLuint program, std::ostream &infoLog);

const char *get_shader_stage_name(GLenum stage);

struct program_deleter {
  void operator()(GLuint obj) { glDeleteProgram(obj); }
};

struct shader_deleter {
  void operator()(GLuint obj) { glDeleteShader(obj); }
};

class shader {
public:
  GLuint object() const { return obj_.get(); }
  static shader compile(GLenum stage, const char *source);
  bool get_compile_status();
  std::string get_compile_log();

private:
  gl_handle<shader_deleter> obj_;
};

struct shader_pp_define {
  const char *define;
  const char *value;
};

class program {
public:
	program() = default;
	program(GLuint obj) : obj_{ obj }
	{}

  GLuint object() const { return obj_.get(); }
  void attach(shader &sh);
  void link();
  bool get_link_status();
  std::string get_link_log();

  static program create(const char *vs_src, const char *fs_src,
                        const char *gs_src = nullptr,
                        const char *tcs_src = nullptr,
                        const char *tes_src = nullptr);
  static program create_compute(const char *cs_src);

  // helper to create a program object from a single-source shader file
  static program load_from_file(
      const std::experimental::filesystem::path &combined_source_file_path,
      shader_stage stages, std::initializer_list<shader_pp_define> defines);

private:
  gl_handle<program_deleter> obj_;
};
}