// Shared ImGui utilities
#pragma once
#include <autograph/engine/Meta.h>

namespace ag {
namespace gui {
AG_ENGINE_API void beginFrame();
AG_ENGINE_API void endFrame();

AG_ENGINE_API void inputTextString(const char *label, std::string &str,
                     size_t buf_size = 100);

AG_ENGINE_API void inputTextMultilineString(const char *label, std::string &str,
                              size_t buf_size = 100);

using NameValuePair = std::pair<const char *, int>;

AG_ENGINE_API void enumComboBox(const char *label, int *outValue,
                  span<const std::pair<const char *, int>> values);

template <typename T>
void enumComboBoxT(const char *label, T *outValue,
                   span<const std::pair<const char *, int>> values) {
  return enumComboBox(label, reinterpret_cast<int *>(outValue), values);
}

template <typename T> void genericValue(T &value) {
  genericValue(typeid(T), &value);
}

 void genericValue(std::type_index ti, void *value);
}
}