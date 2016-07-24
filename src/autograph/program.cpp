#include "program.hpp"
#include "shader_preprocessor.hpp"
#include <iostream>
#include <ostream>

namespace ag {

void dump_compile_log(shader &sh, GLenum stage, std::ostream &out) {
  auto status = sh.get_compile_status();
  auto log = sh.get_compile_log();
  if (!status) {
    out << "===============================================================\n";
    out << "Shader compilation error (stage:" << get_shader_stage_name(stage)
        << ")\n";
    out << "Compilation log follows:\n\n" << log << "\n\n";

  } else if (!log.empty()) {
    out << "Shader compilation messages: (stage:"
        << get_shader_stage_name(stage) << "):\n\n" << log << "\n\n";
  }
}

void dump_link_log(program &prog, std::ostream &out) {
  auto status = prog.get_link_status();
  auto log = prog.get_link_log();
  if (!status) {
    out << "===============================================================\n";
    out << "Program link error\n";
    out << "Link log follows:\n\n" << log << "\n\n";
  } else if (!log.empty()) {
    out << "Program link messages:\n\n" << log << "\n\n";
  }
}

shader shader::compile(GLenum stage, const char *source) {
  shader res;
  GLuint obj = glCreateShader(stage);
  const char *shaderSources[1] = {source};
  glShaderSource(obj, 1, shaderSources, NULL);
  glCompileShader(obj);
  res.obj_ = obj;
  return res;
}

bool shader::get_compile_status()
{
  GLint status = GL_TRUE;
  glGetShaderiv(obj_.get(), GL_COMPILE_STATUS, &status);
  return status == GL_TRUE;
}

std::string shader::get_compile_log()
{
  std::string out;
  GLuint shader = obj_.get();
  GLint logsize = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);
  if (logsize != 0) {
    char *logbuf = new char[logsize];
    glGetShaderInfoLog(shader, logsize, &logsize, logbuf);
    out.assign(logbuf);
    delete[] logbuf;
  }
  return out;
}

bool program::get_link_status()
{
  GLint status = GL_TRUE;
  glGetShaderiv(obj_.get(), GL_LINK_STATUS, &status);
  return status == GL_TRUE;
}

std::string program::get_link_log()
{
  std::string out;
  GLuint program = obj_.get();
  GLint logsize = 0;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize);
  if (logsize != 0) {
    char *logbuf = new char[logsize];
    glGetProgramInfoLog(program, logsize, &logsize, logbuf);
    out.assign(logbuf);
    delete[] logbuf;
  }
  return out;
}

void program::attach(shader &sh) {
  if (!obj_)
    obj_ = glCreateProgram();
  glAttachShader(obj_.get(), sh.object());
}

void program::link() { glLinkProgram(obj_.get()); }

const char *get_shader_stage_name(GLenum stage) {
  switch (stage) {
  case GL_VERTEX_SHADER:
    return "VERTEX_SHADER";
  case GL_FRAGMENT_SHADER:
    return "FRAGMENT_SHADER";
  case GL_GEOMETRY_SHADER:
    return "GEOMETRY_SHADER";
  case GL_TESS_CONTROL_SHADER:
    return "TESS_CONTROL_SHADER";
  case GL_TESS_EVALUATION_SHADER:
    return "TESS_EVALUATION_SHADER";
  case GL_COMPUTE_SHADER:
    return "COMPUTE_SHADER";
  default:
    return "<invalid>";
  }
}

program program::create_compute(const char *cs_src) {
  auto s = shader::compile(GL_COMPUTE_SHADER, cs_src);
  dump_compile_log(s, GL_COMPUTE_SHADER, std::cerr);
  program p;
  p.attach(s);
  p.link();
  dump_link_log(p, std::cerr);
  return p;
}

program program::create(const char *vs_src, const char *fs_src,
                        const char *gs_src, const char *tcs_src,
                        const char *tes_src) {
  auto checked_compile = [](const char *src, GLenum stage) {
    auto s = shader::compile(stage, src);
    dump_compile_log(s, stage, std::cerr);
    return s;
  };

  shader vs, fs, gs, tcs, tes;
  program prog;
  vs = checked_compile(vs_src, GL_VERTEX_SHADER);
  prog.attach(vs);
  fs = checked_compile(fs_src, GL_FRAGMENT_SHADER);
  prog.attach(fs);
  if (gs_src) {
    gs = checked_compile(gs_src, GL_GEOMETRY_SHADER);
    prog.attach(gs);
  }
  if (tes_src) {
    tes = checked_compile(tes_src, GL_TESS_EVALUATION_SHADER);
    prog.attach(tes);
  }
  if (tcs_src) {
    tes = checked_compile(tes_src, GL_TESS_CONTROL_SHADER);
    prog.attach(tcs);
  }

  prog.link();
  dump_link_log(prog, std::cerr);
  return prog;
}

auto make_defines_c_strings(std::vector<shader_pp_define> defines,
                            std::vector<std::string> &out_str) {
  std::vector<const char *> c_strings;
  for (auto &&d : defines) {
    auto str = std::string(d.define);
    if (strlen(d.value)) {
      str += "=";
      str += d.value;
    }
    out_str.push_back(std::move(str));
  }
  for (auto &&d : out_str) {
    c_strings.push_back(d.c_str());
  }
  return std::move(c_strings);
}

  // helper to create a program object from a single-source shader file
  program program::load_from_file(const std::experimental::filesystem::path& combined_source_file_path, shader_stage stages, std::initializer_list<shader_pp_define> defines)
  {
  	//std::clog << "Loading graphics pipeline: " << src.source_or_file_path << "\n";
    std::vector<std::string> def_str;
    auto def_c_str = make_defines_c_strings(defines, def_str);

    std::string vs_src, fs_src, gs_src, tes_src, tcs_src;

    def_c_str.push_back("_VERTEX_");
    vs_src = shaderpp::preprocess(combined_source_file_path, def_c_str.data(), (int)def_c_str.size());
    def_c_str.pop_back();

    def_c_str.push_back("_FRAGMENT_");
    fs_src = shaderpp::preprocess(combined_source_file_path, def_c_str.data(), (int)def_c_str.size());
    def_c_str.pop_back();

    if (not_empty(stages & shader_stage::geometry)) {
      def_c_str.push_back("_GEOMETRY_");
      gs_src = shaderpp::preprocess(combined_source_file_path, def_c_str.data(), (int)def_c_str.size());
      def_c_str.pop_back();
    }

    if (not_empty(stages & shader_stage::tess_control)) {
      def_c_str.push_back("_TESS_CONTROL_");
      tcs_src = shaderpp::preprocess(combined_source_file_path, def_c_str.data(), (int)def_c_str.size());
      def_c_str.pop_back();
    }

    if (not_empty(stages & shader_stage::tess_eval)) {
      def_c_str.push_back("_TESS_EVAL_");
      tes_src = shaderpp::preprocess(combined_source_file_path, def_c_str.data(), (int)def_c_str.size());
      def_c_str.pop_back();
    }

    return program::create(vs_src.c_str(), fs_src.c_str(),
                                gs_src.size() ? gs_src.c_str() : nullptr,
                                tcs_src.size() ? tcs_src.c_str() : nullptr,
                                tes_src.size() ? tes_src.c_str() : nullptr);
  }

}