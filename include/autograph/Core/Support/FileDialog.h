#pragma once
#include <autograph/Core/Exports.h>
#include <autograph/Core/Support/Optional.h>
#include <string>
#include <vector>

namespace ag {
AG_CORE_API optional<std::string> openFileDialog(const char *filters,
                                            const char *defaultPath);
AG_CORE_API optional<std::vector<std::string>>
openFileDialogMultiple(const char *filters, const char *defaultPath);
AG_CORE_API optional<std::string> saveFileDialog(const char *filters,
                                            const char *defaultPath);
AG_CORE_API optional<std::string> pickFolderDialog(const char *defaultPath);
}
