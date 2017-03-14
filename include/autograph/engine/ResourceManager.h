#pragma once
#include <autograph/engine/Config.h>
#include <autograph/support/Span.h>
#include <experimental/filesystem>

namespace ag {

// Uniquely identifies a resource on the filesystem, on the network, in an
// archive file, or in memory
/*class  ResourceID {
public:
  // empty resource ID
  ResourceID() {}
  ResourceID(const char *str) { parseId(str); }
  ResourceID(const std::experimental::filesystem::path &p) : path{p.string()} {}

  // returns true if the resource exists
  bool isValid();
  // returns true if the ID corresponds to a file in the filesystem
  bool isFilesystemResource();
  // returns true if the ID refers to a file in the resource store
  bool isInternalResource();
  //
  auto getResourcePath() -> std::string;
  //
  auto getString() -> std::string { return path; }
  // get the internal part
  auto getSubpath() -> std::string {
    if (privPart != -1) {
      return path.substr(privPart + 1);
    } else {
      return {};
    }
  }
  //
  auto getCStr() const -> const char * { return path.c_str(); }
  //
  auto getString() -> std::string { return path; }
  //
  // auto getString() const -> const std::string & { return str_; }

private:
  void parseId(const char *ch);
  std::string path;
  int privPart = -1;
};*/


namespace ResourceManager {

// returns a filesystem path from the given resource ID
// if the ID identifies a file on the filesystem
// returns the empty string if the ID does not represent a file on the disk
// or if the resource does not exist
AG_ENGINE_API std::string getFilesystemPath(const char *id);
AG_ENGINE_API std::string getFilesystemPath(const std::string& id);

// Same as above, but try the specified prefixes before
AG_ENGINE_API std::string getFilesystemPath(const char *id, span<const char * const> prefixes);
AG_ENGINE_API std::string getFilesystemPath(const std::string& id, span<const char * const> prefixes);

// returns the main path part of the ID, or the empty string if it has none
AG_ENGINE_API std::string getPathPart(const char* id);
AG_ENGINE_API std::string getPathPart(const std::string& id);

// returns the parent directory of the given id
AG_ENGINE_API std::string getParentPath(const char *id);
AG_ENGINE_API std::string getParentPath(const std::string &id);

// returns the subpath part of the ID, or the empty string if it has none
AG_ENGINE_API std::string getSubpathPart(const char* id);
AG_ENGINE_API std::string getSubpathPart(const std::string& id);

// Add a resource directory to look up into when using relative paths
AG_ENGINE_API void addResourceDirectory(const char* fullPath);
AG_ENGINE_API void addResourceDirectory(const std::string& fullPath);

// TODO: getUrl, isUrl, getArchivePath, resolve

// enumerate resource directories
AG_ENGINE_API int getResourceDirectoriesCount();
AG_ENGINE_API std::string getResourceDirectory(int index);
}
}
