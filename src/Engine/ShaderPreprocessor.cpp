#include "ShaderPreprocessor.h"
#include <autograph/Core/Support/Debug.h>
#include <experimental/filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>

namespace ag {

void preprocessMultiShaderSources(ShaderSources &inOutShaderSources,
                                  span<const char *> macros,
                                  span<const char *> includePaths) {
  auto preprocessStage = [&](ShaderSource &inOutSrc, ShaderStage stage) {
    inOutSrc.source = preprocessShaderSource(
        inOutSrc.source, inOutSrc.path.c_str(), stage, macros, includePaths);
  };

  preprocessStage(inOutShaderSources.vertexShader, ShaderStage::Vertex);
  preprocessStage(inOutShaderSources.geometryShader, ShaderStage::Geometry);
  preprocessStage(inOutShaderSources.fragmentShader, ShaderStage::Fragment);
  preprocessStage(inOutShaderSources.tessControlShader,
                  ShaderStage::TessControl);
  preprocessStage(inOutShaderSources.tessEvalShader, ShaderStage::TessEval);
  preprocessStage(inOutShaderSources.computeShader, ShaderStage::Compute);
}

struct IncludeFile {
  const IncludeFile *parent;
  const char *path;
};

using GLSLShaderSourceMap = std::vector<std::tuple<int, std::string>>;

static void preprocessGLSL(std::ostringstream &out, string_view source,
                           int &lastSeenVersion,
                           ShaderStage &enabledShaderStages,
                           const IncludeFile &thisFile,
                           GLSLShaderSourceMap &sourceMap) {
  // update sourcemap
  AG_DEBUG("PP: input:");
  AG_DEBUG("{}", source.substr(0));

  int thisFileIndex = sourceMap.size();
  sourceMap.push_back({(int)sourceMap.size(), thisFile.path});

  static const std::regex directivesRegex{
      R""(^\s*#include\s+"(.*)"\s*?$|^\s*#version\s+([0-9]*)\s*?$|^\s*#pragma\s+(.*)\s*?$|^(.*)$)""};
  namespace fs = std::experimental::filesystem;

  fs::path thisFilePath{thisFile.path};
  fs::path parentPath = thisFilePath.parent_path();

  // find (and remove) version directives
  using regex_iterator = std::regex_iterator<string_view::iterator>;
  using match = std::match_results<string_view::iterator>;

  auto directives_begin =
      regex_iterator{source.begin(), source.end(), directivesRegex};
  auto directives_end = regex_iterator{};

  int curLine = 1;
  bool shouldOutputLineDirective = false;

  for (regex_iterator i = directives_begin; i != directives_end; ++i) {
    match m = *i;
    if (m[1].matched) {
      AG_DEBUG("PP: matched include directive");
      ////////////////////////////////////////
      // matched include directive
      auto includePath = m[1].str();
      // prepend
      includePath = (parentPath / includePath).string();
      // try to open the file
      std::ifstream fileIn{includePath, std::ios::in};
      if (!fileIn.is_open()) {
        errorMessage("[{}] line {}: could not open include file {}",
                     thisFile.path, curLine, includePath.c_str());
      } else {
        // read its contents
        std::string str;
        str.assign((std::istreambuf_iterator<char>(fileIn)),
                   std::istreambuf_iterator<char>());
        // create IncludeFile
        IncludeFile inc{&thisFile, includePath.c_str()};
        // recurse
        preprocessGLSL(out, str, lastSeenVersion, enabledShaderStages, inc,
                       sourceMap);
      }

      shouldOutputLineDirective = true;
      curLine++;
    } else if (m[2].matched) {
      AG_DEBUG("PP: matched version directive");
      ////////////////////////////////////////
      // matched version directive
      auto versionStr = m[2].str();
      try {
        int version = std::stoi(versionStr);
        lastSeenVersion = version;
      } catch (std::invalid_argument &) {
        warningMessage("[{}] line {}: malformed #version directive",
                       thisFile.path, curLine);
      }
      shouldOutputLineDirective = true;
      curLine++;
    } else if (m[3].matched) {
      ////////////////////////////////////////
      // matched pragma directive
      AG_DEBUG("PP: matched pragma directive");
      static const std::regex shaderStagePragmaRegexp{
          R""(^stages\s*\(\s*(\w+)\s*(?:\s*,\s*(\w+))*\s*\)\s*?$)""};
      std::cmatch matches;
      if (std::regex_match(m[3].str().c_str(), matches,
                           shaderStagePragmaRegexp)) {
        for (int matchIndex = 1; matchIndex < matches.size(); ++matchIndex) {
          if (matches[matchIndex] == "vertex") {
            enabledShaderStages |= ShaderStage::Vertex;
          } else if (matches[matchIndex] == "fragment") {
            enabledShaderStages |= ShaderStage::Fragment;
          } else if (matches[matchIndex] == "geometry") {
            enabledShaderStages |= ShaderStage::Geometry;
          } else if (matches[matchIndex] == "tess_control") {
            enabledShaderStages |= ShaderStage::TessControl;
          } else if (matches[matchIndex] == "tess_eval") {
            enabledShaderStages |= ShaderStage::TessEval;
          } else if (matches[matchIndex] == "compute") {
            enabledShaderStages |= ShaderStage::Compute;
          } else if (matches[matchIndex].matched) {
            warningMessage(
                "Unknown shader stage in #pragma stages(...) directive: {}",
                matches[matchIndex]);
          }
        }
      } else {
        warningMessage("Ignored #pragma directive: {}", m[3].str());
      }
    } else if (m[4].matched) {
      ////////////////////////////////////////
      // matched line
      AG_DEBUG("PP: matched line");
      if (shouldOutputLineDirective) {
        out << "#line " << curLine << " " << thisFileIndex << '\n';
        shouldOutputLineDirective = false;
      }
      out << m[4].str() << '\n';
      curLine++;
    }
  }
}

std::string preprocessShaderSource(string_view source, const char *path,
                                   ShaderStage stage, span<const char *> macros,
                                   span<const char *> includePaths) {

  IncludeFile thisFile{nullptr, path};
  std::ostringstream outBody;
  int version = 0;
  GLSLShaderSourceMap sourceMap;
  ShaderStage enabledShaderStages = (ShaderStage)0;
  preprocessGLSL(outBody, source, version, enabledShaderStages, thisFile,
                 sourceMap);
  AG_DEBUG("PP: Enabled stages: {0:#x}", (int)enabledShaderStages);
  // This source does not define a shader of the specified type
  if (!(enabledShaderStages & stage)) {
    return {};
  }
  if (!version) {
    warningMessage("No #version directive found while preprocessing: "
                   "defaulting to version 3.30");
    version = 330;
  }
  // Debug infos
  AG_DEBUG("Detected version: {}", version);
  AG_DEBUG("Source map:");
  for (const auto &s : sourceMap) {
    AG_DEBUG("     {} -> {}", std::get<int>(s), std::get<std::string>(s));
  }
  std::ostringstream outHeader;
  int numHeaderLines = 1;
  outHeader << "#version " << version << '\n';
  for (auto macrostr : macros) {
    static const std::regex macroRegex{R""(^(\w+)(?:=(\w*))?$)""};
    std::cmatch matches;
    if (std::regex_match(macrostr, matches, macroRegex)) {
      outHeader << "#define " << matches[1].str();
      if (matches[2].matched) {
        outHeader << " " << matches[2].str();
      }
      outHeader << '\n';
      ++numHeaderLines;
    }
  }

  switch (stage) {
  case ShaderStage::Vertex:
    outHeader << "#define _VERTEX_\n";
    break;
  case ShaderStage::Geometry:
    outHeader << "#define _GEOMETRY_\n";
    break;
  case ShaderStage::Fragment:
    outHeader << "#define _FRAGMENT_\n";
    break;
  case ShaderStage::TessControl:
    outHeader << "#define _TESS_CONTROL_\n";
    break;
  case ShaderStage::TessEval:
    outHeader << "#define _TESS_EVAL_\n";
    break;
  case ShaderStage::Compute:
    outHeader << "#define _COMPUTE_\n";
    break;
  }
  ++numHeaderLines;
  //
  outHeader << "#line " << numHeaderLines << " " << 0 << "\n";

  return outHeader.str() + outBody.str();
}
} // namespace ag