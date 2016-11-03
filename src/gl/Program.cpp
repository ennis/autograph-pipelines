#include <autograph/gl/Program.h>
#include <autograph/gl/ShaderPreprocessor.h>
#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>

namespace ag {
namespace gl {

void dumpCompileLog(Shader &sh, GLenum stage, std::ostream &out) {
  auto status = sh.getCompileStatus();
  auto log = sh.getCompileLog();
  if (!status) {
    out << "===============================================================\n";
    out << "Shader compilation error (stage:" << getShaderStageName(stage)
        << ")\n";
    out << "Compilation log follows:\n\n" << log << "\n\n";

  } else if (!log.empty()) {
    out << "Shader compilation messages: (stage:" << getShaderStageName(stage)
        << "):\n\n"
        << log << "\n\n";
  }
}

void dump_link_log(Program &prog, std::ostream &out) {
  auto status = prog.getLinkStatus();
  auto log = prog.getLinkLog();
  if (!status) {
    out << "===============================================================\n";
    out << "Program link error\n";
    out << "Link log follows:\n\n" << log << "\n\n";
  } else if (!log.empty()) {
    out << "Program link messages:\n\n" << log << "\n\n";
  }
}

Shader Shader::compile(GLenum stage, const char *source) {
  Shader res;
  GLuint obj = glCreateShader(stage);
  const char *shaderSources[1] = {source};
  glShaderSource(obj, 1, shaderSources, NULL);
  glCompileShader(obj);
  res.obj_ = obj;
  return res;
}

bool Shader::getCompileStatus() {
  GLint status = GL_TRUE;
  glGetShaderiv(obj_.get(), GL_COMPILE_STATUS, &status);
  return status == GL_TRUE;
}

std::string Shader::getCompileLog() {
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

bool Program::getLinkStatus() {
  GLint status = GL_TRUE;
  glGetShaderiv(obj_.get(), GL_LINK_STATUS, &status);
  return status == GL_TRUE;
}

std::string Program::getLinkLog() {
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

void Program::attach(Shader &sh) {
  if (!obj_)
    obj_ = glCreateProgram();
  glAttachShader(obj_.get(), sh.object());
}

void Program::link() { glLinkProgram(obj_.get()); }

const char *getShaderStageName(GLenum stage) {
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

Program Program::createCompute(const char *cs_src) {
  auto s = Shader::compile(GL_COMPUTE_SHADER, cs_src);
  dumpCompileLog(s, GL_COMPUTE_SHADER, std::cerr);
  Program p;
  p.attach(s);
  p.link();
  dump_link_log(p, std::cerr);
  return p;
}

Program Program::create(const char *vs_src, const char *fs_src,
                        const char *gs_src, const char *tcs_src,
                        const char *tes_src) {
  auto checkedCompile = [](const char *src, GLenum stage) {
    auto s = Shader::compile(stage, src);
    dumpCompileLog(s, stage, std::cerr);
    return s;
  };

  Shader vs, fs, gs, tcs, tes;
  Program prog;
  vs = checkedCompile(vs_src, GL_VERTEX_SHADER);
  prog.attach(vs);
  fs = checkedCompile(fs_src, GL_FRAGMENT_SHADER);
  prog.attach(fs);
  if (gs_src) {
    gs = checkedCompile(gs_src, GL_GEOMETRY_SHADER);
    prog.attach(gs);
  }
  if (tes_src) {
    tes = checkedCompile(tes_src, GL_TESS_EVALUATION_SHADER);
    prog.attach(tes);
  }
  if (tcs_src) {
    tes = checkedCompile(tes_src, GL_TESS_CONTROL_SHADER);
    prog.attach(tcs);
  }

  prog.link();
  dump_link_log(prog, std::cerr);
  return prog;
}

auto makeDefineStrings(int numDefines, ShaderPPDefine* pDefines,
                       std::vector<std::string> &out_str) {
  std::vector<const char *> c_strings;
  for (int i = 0; i < numDefines; ++i)
  { 
	  auto str = std::string{ pDefines[i].define };
    if (strlen(pDefines[i].value)) {
      str += "=";
      str += pDefines[i].value;
    }
    out_str.push_back(std::move(str));
  }
  for (auto &&d : out_str) {
    c_strings.push_back(d.c_str());
  }
  return c_strings;
}

// helper to create a program object from a single-source shader file
Program Program::loadFromFile(
    const char* combined_source_file_path,
    ShaderStage stages, int numDefines, ShaderPPDefine* pDefines) {
  // std::clog << "Loading graphics pipeline: " << src.source_or_file_path <<
  // "\n";
  std::vector<std::string> def_str;
  auto def_c_str = makeDefineStrings(numDefines, pDefines, def_str);

  std::string vs_src, fs_src, gs_src, tes_src, tcs_src;

  def_c_str.push_back("_VERTEX_");
  vs_src = preprocessShaderFile(combined_source_file_path, def_c_str.data(),
                                (int)def_c_str.size());
  def_c_str.pop_back();

  def_c_str.push_back("_FRAGMENT_");
  fs_src = preprocessShaderFile(combined_source_file_path, def_c_str.data(),
                                (int)def_c_str.size());
  def_c_str.pop_back();

  if (not_empty(stages & ShaderStage::Geometry)) {
    def_c_str.push_back("_GEOMETRY_");
    gs_src = preprocessShaderFile(combined_source_file_path, def_c_str.data(),
                                  (int)def_c_str.size());
    def_c_str.pop_back();
  }

  if (not_empty(stages & ShaderStage::TessControl)) {
    def_c_str.push_back("_TESS_CONTROL_");
    tcs_src = preprocessShaderFile(combined_source_file_path, def_c_str.data(),
                                   (int)def_c_str.size());
    def_c_str.pop_back();
  }

  if (not_empty(stages & ShaderStage::TessEval)) {
    def_c_str.push_back("_TESS_EVAL_");
    tes_src = preprocessShaderFile(combined_source_file_path, def_c_str.data(),
                                   (int)def_c_str.size());
    def_c_str.pop_back();
  }

  return Program::create(vs_src.c_str(), fs_src.c_str(),
                         gs_src.size() ? gs_src.c_str() : nullptr,
                         tcs_src.size() ? tcs_src.c_str() : nullptr,
                         tes_src.size() ? tes_src.c_str() : nullptr);
}
}
}