#include <autograph/support/FileDialog.h>
#include <nfd.h>

namespace ag {
optional<std::string> openFileDialog(const char *filters,
                                     const char *defaultPath) {
  nfdchar_t *outpath = nullptr;
  auto res = NFD_OpenDialog(filters, defaultPath, &outpath);
  if (res == NFD_OKAY)
    return std::string{outpath};
  return nullopt;
}

optional<std::vector<std::string>>
openFileDialogMultiple(const char *filters, const char *defaultPath) {
  nfdpathset_t pathset;
  auto res = NFD_OpenDialogMultiple(filters, defaultPath, &pathset);
  if (res == NFD_OKAY) {
    std::vector<std::string> out;
    int pathCount = NFD_PathSet_GetCount(&pathset);
    for (int i = 0; i < pathCount; ++i) {
      out.push_back(std::string{NFD_PathSet_GetPath(&pathset, i)});
    }
    return out;
  }
  return nullopt;
}

optional<std::string> saveFileDialog(const char *filters,
                                     const char *defaultPath) {
  nfdchar_t *outpath = nullptr;
  auto res = NFD_SaveDialog(filters, defaultPath, &outpath);
  if (res == NFD_OKAY)
    return std::string{outpath};
  return nullopt;
}

optional<std::string> pickFolderDialog(const char *defaultPath) {
  nfdchar_t *outpath = nullptr;
  auto res = NFD_PickFolder(defaultPath, &outpath);
  if (res == NFD_OKAY)
    return std::string{outpath};
  return nullopt;
}
}