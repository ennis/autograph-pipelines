#include <autograph/Gfx/Program.h>
#include <autograph/Core/Support/Debug.h>
#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>

namespace ag {

void dumpCompileLog(ShaderObject &sh, gl::GLenum stage, std::ostream &out,
                    const char *fileHint = "<unknown>") {
  auto status = sh.getCompileStatus();
  auto log = sh.getCompileLog();
  if (!status) {
    errorMessage(
        "===============================================================");
    errorMessage("Shader compilation error (file: {}, stage: {})", fileHint,
                 getShaderStageName(stage));
    errorMessage("Compilation log follows:\n\n{}\n\n", log);

  } else if (!log.empty()) {
    warningMessage("Shader compilation messages (file: {}, stage: {})",
                   fileHint, getShaderStageName(stage));
    warningMessage("{}", log);
  }
}

void dumpLinkLog(ProgramObject &prog, std::ostream &out,
                 const char *fileHint = "<unknown>") {
  auto status = prog.getLinkStatus();
  auto log = prog.getLinkLog();
  if (!status) {
    errorMessage(
        "===============================================================");
    errorMessage("Program link error");
    errorMessage("Link log follows:\n\n{}\n\n", log);
  } else if (!log.empty()) {
    warningMessage("Program link messages:");
    warningMessage("{}", log);
  }
}

ShaderObject ShaderObject::compile(gl::GLenum stage, const char *source) {
  ShaderObject res;
  gl::GLuint obj = gl::CreateShader(stage);
  const char *shaderSources[1] = {source};
  gl::ShaderSource(obj, 1, shaderSources, NULL);
  gl::CompileShader(obj);
  res.obj_ = obj;
  return res;
}

bool ShaderObject::getCompileStatus() {
  gl::GLint status = gl::TRUE_;
  gl::GetShaderiv(obj_.get(), gl::COMPILE_STATUS, &status);
  return status == gl::TRUE_;
}

std::string ShaderObject::getCompileLog() {
  std::string out;
  gl::GLuint shader = obj_.get();
  gl::GLint logsize = 0;
  gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &logsize);
  if (logsize != 0) {
    char *logbuf = new char[logsize];
    gl::GetShaderInfoLog(shader, logsize, &logsize, logbuf);
    out.assign(logbuf);
    delete[] logbuf;
  }
  return out;
}

bool ProgramObject::getLinkStatus() {
  gl::GLint status = gl::TRUE_;
  gl::GetProgramiv(obj_.get(), gl::LINK_STATUS, &status);
  return status == gl::TRUE_;
}

std::string ProgramObject::getLinkLog() {
  std::string out;
  gl::GLuint program = obj_.get();
  gl::GLint logsize = 0;
  gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &logsize);
  if (logsize != 0) {
    char *logbuf = new char[logsize];
    gl::GetProgramInfoLog(program, logsize, &logsize, logbuf);
    out.assign(logbuf);
    delete[] logbuf;
  }
  return out;
}

void ProgramObject::attach(ShaderObject &sh) {
  if (!obj_)
    obj_ = gl::CreateProgram();
  gl::AttachShader(obj_.get(), sh.object());
}

void ProgramObject::link() { gl::LinkProgram(obj_.get()); }

const char *getShaderStageName(gl::GLenum stage) {
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

ProgramObject ProgramObject::createCompute(const char *cs_src) {
  auto s = ShaderObject::compile(gl::COMPUTE_SHADER, cs_src);
  dumpCompileLog(s, gl::COMPUTE_SHADER, std::cerr);
  ProgramObject p;
  p.attach(s);
  p.link();
  dumpLinkLog(p, std::cerr);
  return p;
}

ProgramObject ProgramObject::create(const char *vs_src, const char *fs_src,
                                    const char *gs_src, const char *tcs_src,
                                    const char *tes_src) {
  auto checkedCompile = [](const char *src, gl::GLenum stage) {
    auto s = ShaderObject::compile(stage, src);
    dumpCompileLog(s, stage, std::cerr);
    return s;
  };

  ShaderObject vs, fs, gs, tcs, tes;
  ProgramObject prog;
  vs = checkedCompile(vs_src, gl::VERTEX_SHADER);
  prog.attach(vs);
  fs = checkedCompile(fs_src, gl::FRAGMENT_SHADER);
  prog.attach(fs);
  if (gs_src) {
    gs = checkedCompile(gs_src, gl::GEOMETRY_SHADER);
    prog.attach(gs);
  }
  if (tes_src) {
    tes = checkedCompile(tes_src, gl::TESS_EVALUATION_SHADER);
    prog.attach(tes);
  }
  if (tcs_src) {
    tcs = checkedCompile(tcs_src, gl::TESS_CONTROL_SHADER);
    prog.attach(tcs);
  }

  prog.link();
  dumpLinkLog(prog, std::cerr);
  return prog;
}

/*auto makeDefineStrings(int numDefines, ShaderPPDefine* pDefines,
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
}*/

// helper to create a program object from a single-source shader file
/*Program Program::loadFromFile(
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
}*/
} // namespace ag
