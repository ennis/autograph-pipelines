#include "MurmurHash3.h"
#include "pipeline_program.hpp"
#include "shader_preprocessor.hpp"

#include <experimental/filesystem>
#include <format.h>
#include <gsl.h>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace {

GLuint compile_shader(GLenum stage, const char *source, std::ostream &infoLog) {
  GLuint obj = gl::CreateShader(stage);
  const char *shaderSources[1] = {source};
  gl::ShaderSource(obj, 1, shaderSources, NULL);
  gl::CompileShader(obj);
  GLint status = gl::TRUE_;
  GLint logsize = 0;
  gl::GetShaderiv(obj, gl::COMPILE_STATUS, &status);
  gl::GetShaderiv(obj, gl::INFO_LOG_LENGTH, &logsize);
  if (status != gl::TRUE_) {
    if (logsize != 0) {
      char *logbuf = new char[logsize];
      gl::GetShaderInfoLog(obj, logsize, &logsize, logbuf);
      infoLog << logbuf;
      delete[] logbuf;
      gl::DeleteShader(obj);
    }
    return 0;
  }
  return obj;
}

bool link_program(GLuint program, std::ostream &infoLog) {
  GLint status = gl::TRUE_;
  GLint logsize = 0;
  gl::LinkProgram(program);
  gl::GetProgramiv(program, gl::LINK_STATUS, &status);
  gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &logsize);
  if (status != gl::TRUE_) {
    if (logsize != 0) {
      char *logbuf = new char[logsize];
      gl::GetProgramInfoLog(program, logsize, &logsize, logbuf);
      infoLog << logbuf;
      delete[] logbuf;
    }
    return true;
  }
  return false;
}

const char *get_shader_stage_name(GLenum stage) {
  switch (stage) {
  case gl::VERTEX_SHADER:
    return "VERTEX_SHADER";
  case gl::FRAGMENT_SHADER:
    return "FRAGMENT_SHADER";
  case gl::GEOMETRY_SHADER:
    return "GEOMETRY_SHADER";
  case gl::TESS_CONTROL_SHADER:
    return "TESS_CONTROL_SHADER";
  case gl::TESS_EVALUATION_SHADER:
    return "TESS_EVALUATION_SHADER";
  case gl::COMPUTE_SHADER:
    return "COMPUTE_SHADER";
  default:
    return "<invalid>";
  }
}

GLuint compile_and_attach_shader(GLuint program_obj, GLenum stage,
                                 const char *source) {
  std::ostringstream infoLog;
  GLuint shader_obj = compile_shader(stage, source, infoLog);
  if (!shader_obj) {
    std::clog
        << "===============================================================\n";
    std::clog << "Shader compilation error (stage:"
              << get_shader_stage_name(stage) << ")\n";
    std::clog << "Compilation log follows:\n\n" << infoLog.str() << "\n\n";
    return 0;
  }
  gl::AttachShader(program_obj, shader_obj);
  return shader_obj;
}

GLuint create_compute_program(const char *src) {
  GLuint cs_obj = 0;
  GLuint program_obj = gl::CreateProgram();
  cs_obj = compile_and_attach_shader(program_obj, gl::COMPUTE_SHADER, src);
  if (!cs_obj) {
    gl::DeleteProgram(program_obj);
    return 0;
  }
  std::ostringstream linkInfoLog;
  bool link_error = link_program(program_obj, linkInfoLog);
  gl::DetachShader(program_obj, cs_obj);
  gl::DeleteShader(cs_obj);
  if (link_error) {
    std::clog
        << "===============================================================\n";
    std::clog << "Shader link error\n";
    std::clog << "Compilation log follows:\n\n" << linkInfoLog.str() << "\n\n";
    gl::DeleteProgram(program_obj);
    program_obj = 0;
  }
  return program_obj;
}

GLuint create_graphics_program(const char *vs_src, const char *fs_src,
                               const char *gs_src, const char *tcs_src,
                               const char *tes_src) {
  GLuint vs_obj = 0;
  GLuint fs_obj = 0;
  GLuint gs_obj = 0;
  GLuint tcs_obj = 0;
  GLuint tes_obj = 0;
  GLuint program_obj = gl::CreateProgram();
  bool compilation_error = false;
  vs_obj = compile_and_attach_shader(program_obj, gl::VERTEX_SHADER, vs_src);
  if (!vs_obj)
    compilation_error = true;
  fs_obj = compile_and_attach_shader(program_obj, gl::FRAGMENT_SHADER, fs_src);
  if (!fs_obj)
    compilation_error = true;
  if (gs_src) {
    gs_obj =
        compile_and_attach_shader(program_obj, gl::GEOMETRY_SHADER, gs_src);
    if (!gs_obj)
      compilation_error = true;
  }
  if (tes_src) {
    tes_obj = compile_and_attach_shader(program_obj, gl::TESS_EVALUATION_SHADER,
                                        tes_src);
    if (!tes_obj)
      compilation_error = true;
  }
  if (tcs_src) {
    tcs_obj = compile_and_attach_shader(program_obj, gl::TESS_CONTROL_SHADER,
                                        tcs_src);
    if (!tcs_obj)
      compilation_error = true;
  }

  bool link_error = false;
  if (!compilation_error) {
    std::ostringstream linkInfoLog;
    link_error = link_program(program_obj, linkInfoLog);
    if (link_error) {
      std::clog << "==========================================================="
                   "====\n";
      std::clog << "Shader link error\n";
      std::clog << "Compilation log follows:\n\n"
                << linkInfoLog.str() << "\n\n";
    }
  }

  if (vs_obj)
    gl::DeleteShader(vs_obj);
  if (fs_obj)
    gl::DeleteShader(fs_obj);
  if (gs_obj)
    gl::DeleteShader(gs_obj);
  if (tcs_obj)
    gl::DeleteShader(tcs_obj);
  if (tes_obj)
    gl::DeleteShader(tes_obj);
  if (link_error) {
    gl::DeleteProgram(program_obj);
    program_obj = 0;
  }

  return program_obj;
}

GLuint createVertexArrayObject(gsl::span<const gl_vertex_attribute> attribs) {
  GLuint strides[gl_max_vertex_buffers] = {0};
  GLuint vertex_array_obj;
  gl::CreateVertexArrays(1, &vertex_array_obj);
  assert(attribs.size() < gl_max_vertex_attributes);
  for (int attribindex = 0; attribindex < attribs.size(); ++attribindex) {
    const auto &a = attribs[attribindex];
    assert(a.slot < gl_max_vertex_buffers);
    gl::EnableVertexArrayAttrib(vertex_array_obj, attribindex);
    gl::VertexArrayAttribFormat(vertex_array_obj, attribindex, a.size, a.type,
                                a.normalized, strides[a.slot]);
    gl::VertexArrayAttribBinding(vertex_array_obj, attribindex, a.slot);
    strides[a.slot] += a.stride;
  }
  gl::BindVertexArray(0);
  return vertex_array_obj;
}
}

// cache linked programs
struct program_cache {
  program_cache(std::experimental::filesystem::path cache_dir)
      : cachedir(cache_dir) {}

  enum cache_action { load_only, load_and_cache, load_only_from_cache, bypass };

  enum cache_mode {
    // cache/lookup to memory only
    memory,
    // cache to memory AND binary file
    program_binary_file
  };

  GLuint query_cache_compute(const char *src, cache_action action,
                             cache_mode mode) {
    // hash the shader source code
    auto len = std::strlen(src);
    uint64_t hash[2];
    hash[0] = 42;
    hash[1] = 816;
    MurmurHash3_x64_128_incr(src, len, hash);
    MurmurHash3_x64_128_fin(len, hash);
    // try to load program from memory cache
    auto m = cscache.find(hash[0]);
    if (m != cscache.end()) {
      return m->second;
    }
    // not found in memory cache
    // try to load program binary from the cache directory
    std::string hashstr;
    if (mode == cache_mode::program_binary_file) {
      hashstr = fmt::format("{:016x}", hash[0]);
      auto path = cachedir / (hashstr + ".pbin");
      if (std::experimental::filesystem::is_regular_file(path)) {
        // program found in binary cache, load it
        // GLuint prog = load_program_binary(...)
        // cscache.insert(...)
        // return prog;
      }
    }

    // not found in memory cache or binary directory
    if (action == cache_action::load_only_from_cache)
      return 0;
    // compile the program
    GLuint prog = create_compute_program(src);
    // Exit early if compilation error
    if (prog == 0)
      return 0;
    // Optionally move the program into the caches
    if (action == cache_action::load_and_cache) {
      cscache.insert(std::make_pair(hash[0], prog));
      if (mode == cache_mode::program_binary_file) {
        // save_program_binary(path)
      }
    }
    return prog;
  }

  // Key=64-bit hash of the shader GLSL source
  // TODO should be refcounted
  std::unordered_map<uint64_t, GLuint> cscache;
  std::unordered_map<uint64_t, GLuint> gscache;
  std::experimental::filesystem::path cachedir;
};

static program_cache g_program_cache{"shadercache"};

compute_pipeline_program compute_pipeline_program::compile_from_file(
    const char *file_name_, std::initializer_list<pp_define> defines_) {
  std::vector<std::string> defs;
  std::vector<const char *> pdefs;
  defs.push_back("_COMPUTE_");
  for (auto &&d : defines_) {
    defs.push_back(std::string(d.define) + "=" + d.value);
  }
  for (auto &&d : defs) {
    pdefs.push_back(d.c_str());
  }
  auto src = preprocess(file_name_, pdefs.data(), pdefs.size());
  auto prog = g_program_cache.query_cache_compute(
      src.c_str(), program_cache::cache_action::load_and_cache,
      program_cache::cache_mode::program_binary_file);
  compute_pipeline_program pp;
  pp.src = shader_source{shader_source::location::file, std::move(file_name_)};
  pp.gl_program = prog;
  return std::move(pp);
}

compute_pipeline_program
compute_pipeline_program::compile_from_source(const char *src) {}