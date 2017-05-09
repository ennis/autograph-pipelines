// Shared ImGui utilities
#pragma once
#include <autograph/Engine/Meta.h>
#include <autograph/Gfx/gl_core_4_5.h>
#include <autograph/Engine/imgui.h>

namespace ag {
namespace gui {

inline vec2 toVec2(ImVec2 v) { return vec2{v.x, v.y}; };
inline ImVec2 toImVec2(vec2 v) { return ImVec2{v.x, v.y}; };

AG_ENGINE_API void beginFrame();
AG_ENGINE_API void endFrame();

struct OpenGLState {
  gl::GLint last_program;
  gl::GLint last_texture;
  gl::GLint last_active_texture;
  gl::GLint last_array_buffer;
  gl::GLint last_element_array_buffer;
  gl::GLint last_vertex_array;
  gl::GLint last_blend_src;
  gl::GLint last_blend_dst;
  gl::GLint last_blend_equation_rgb;
  gl::GLint last_blend_equation_alpha;
  gl::GLint last_viewport[4];
  gl::GLboolean last_enable_blend;
  gl::GLboolean last_enable_cull_face;
  gl::GLboolean last_enable_depth_test;
  gl::GLboolean last_enable_scissor_test;
};

AG_ENGINE_API void saveOpenGLState(OpenGLState &outSavedState);
AG_ENGINE_API void restoreOpenGLState(const OpenGLState &outSavedState);

template <typename Callback> static void customRendering(Callback callback) {
  // ImGui::Dummy(ImVec2{ (float)w, (float)h });
  auto drawList = ImGui::GetWindowDrawList();
  auto pfn = new std::function<void(const ImDrawList *, const ImDrawCmd *)>{
      std::move(callback)};
  auto drawCallback = [](const ImDrawList *parentList, const ImDrawCmd *cmd) {
    auto pfn = static_cast<
        std::function<void(const ImDrawList *, const ImDrawCmd *)> *>(
        cmd->UserCallbackData);
    OpenGLState savedState;
    saveOpenGLState(savedState);
    (*pfn)(parentList, cmd);
    restoreOpenGLState(savedState);
    delete pfn;
  };
  drawList->AddCallback(drawCallback, pfn);
}

AG_ENGINE_API void inputTextString(const char *label, std::string &str,
                                   size_t buf_size = 100);

AG_ENGINE_API void inputTextMultilineString(const char *label, std::string &str,
                                            size_t buf_size = 100);

using NameValuePair = std::pair<const char *, int>;

AG_ENGINE_API void
enumComboBox(const char *label, int *outValue,
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
} // namespace gui
} // namespace ag