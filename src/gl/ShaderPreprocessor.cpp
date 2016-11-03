#include <autograph/gl/ShaderPreprocessor.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <mcpp_lib.h>
#include <string>
#include <vector>

namespace ag {
namespace gl {
namespace {
std::locale cloc("C");

std::string load_source_file(const char *path) {
  std::ifstream fileIn(path, std::ios::in);
  if (!fileIn.is_open()) {
    std::cerr << "Could not open file " << path << std::endl;
    throw std::runtime_error("Could not open file");
  }
  std::string str;
  str.assign((std::istreambuf_iterator<char>(fileIn)),
             std::istreambuf_iterator<char>());
  return str;
}

void skipws(const std::string &line, int &pos) {
  // skip whitespace
  while (std::isspace(line[pos], cloc) && pos < line.size()) {
    pos++;
  }
}

void skipnws(const std::string &line, int &pos) {
  // skip whitespace
  while (!std::isspace(line[pos], cloc) && pos < line.size()) {
    pos++;
  }
}

bool path_character(char ch, const std::locale &loc) { return ch != '>'; }

std::string next_word(const std::string &line, int &pos) {
  skipws(line, pos);
  int beg = pos;
  skipnws(line, pos);
  return line.substr(beg, pos - beg);
}
}

std::vector<MultiCompileDirective> getMultiCompileDirectives(
    const char* srcpath) {
  std::vector<MultiCompileDirective> out;
  std::ifstream fileIn(srcpath, std::ios::in);
  std::string line;
  while (std::getline(fileIn, line)) {
    int pos = 0;
    if (line.size() == 0)
      continue;
    if (line[pos] == '#') {
      ++pos;
      auto dir = next_word(line, pos);
      if (dir == "pragma") {
        auto pragma = next_word(line, pos);
        if (pragma == "multi_compile") {
          // std::cerr << "multi_compile: ";
          MultiCompileDirective m;
          for (auto kw = next_word(line, pos); kw.size();
               kw = next_word(line, pos)) {
            m.kw.push_back(kw);
            // std::cerr << kw;
          }
          out.push_back(std::move(m));
        }
      }
    }
  }
  return out;
}

std::string preprocessShaderFile(const char* srcpath,
                       const char **defines, int ndefines) {
  mcpp_use_mem_buffers(1);

  std::vector<std::string> argv_str;
  argv_str.push_back("mcpp");
  for (int i = 0; i < ndefines; ++i) {
    argv_str.push_back("-D");
    argv_str.push_back(defines[i]);
  }
  argv_str.push_back(srcpath);

  std::vector<const char *> argv(argv_str.size());
  for (int i = 0; i < argv_str.size(); ++i) {
    argv[i] = argv_str[i].c_str();
  }

  std::ios_base::sync_with_stdio(false);
  mcpp_lib_main((int)argv.size(), const_cast<char **>(argv.data()));
  const char *buf = mcpp_get_mem_buffer(OUT);
  const char *err = mcpp_get_mem_buffer(ERR);
  std::clog.clear();
  std::clog << "preprocessed" << std::endl;
  return std::string{buf};
}

std::string preprocessShaderFile(const char *srcpath,
                              std::vector<const char *> defs) {
  auto src = preprocessShaderFile(srcpath, defs.data(), (int)defs.size());
  return src;
}
}
}