#include <autograph/Transform.h>
#include <autograph/engine/ImGuiUtils.h>
#include <autograph/engine/ImGuizmo.h>
#include <autograph/engine/RenderUtils.h>
#include <autograph/engine/imgui.h>
#include <autograph/gl/All.h>
#include <autograph/support/FileDialog.h>
#include <autograph/support/ProjectRoot.h>

namespace ag {
namespace gui {

void beginFrame() { ImGuizmo::BeginFrame(); }
void endFrame() {}

void saveOpenGLState(OpenGLState &outSavedState) {
  gl::GetIntegerv(gl::CURRENT_PROGRAM, &outSavedState.last_program);
  gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &outSavedState.last_texture);
  gl::GetIntegerv(gl::ACTIVE_TEXTURE, &outSavedState.last_active_texture);
  gl::GetIntegerv(gl::ARRAY_BUFFER_BINDING, &outSavedState.last_array_buffer);
  gl::GetIntegerv(gl::ELEMENT_ARRAY_BUFFER_BINDING,
                  &outSavedState.last_element_array_buffer);
  gl::GetIntegerv(gl::VERTEX_ARRAY_BINDING, &outSavedState.last_vertex_array);
  gl::GetIntegerv(gl::BLEND_SRC, &outSavedState.last_blend_src);
  gl::GetIntegerv(gl::BLEND_DST, &outSavedState.last_blend_dst);
  gl::GetIntegerv(gl::BLEND_EQUATION_RGB,
                  &outSavedState.last_blend_equation_rgb);
  gl::GetIntegerv(gl::BLEND_EQUATION_ALPHA,
                  &outSavedState.last_blend_equation_alpha);
  gl::GetIntegerv(gl::VIEWPORT, outSavedState.last_viewport);
  outSavedState.last_enable_blend = gl::IsEnabled(gl::BLEND);
  outSavedState.last_enable_cull_face = gl::IsEnabled(gl::CULL_FACE);
  outSavedState.last_enable_depth_test = gl::IsEnabled(gl::DEPTH_TEST);
  outSavedState.last_enable_scissor_test = gl::IsEnabled(gl::SCISSOR_TEST);
}

void restoreOpenGLState(const OpenGLState &savedState) {
  gl::UseProgram(savedState.last_program);
  gl::ActiveTexture(savedState.last_active_texture);
  gl::BindTexture(gl::TEXTURE_2D, savedState.last_texture);
  gl::BindVertexArray(savedState.last_vertex_array);
  gl::BindBuffer(gl::ARRAY_BUFFER, savedState.last_array_buffer);
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER,
                 savedState.last_element_array_buffer);
  gl::BlendEquationSeparate(savedState.last_blend_equation_rgb,
                            savedState.last_blend_equation_alpha);
  gl::BlendFunc(savedState.last_blend_src, savedState.last_blend_dst);
  if (savedState.last_enable_blend)
    gl::Enable(gl::BLEND);
  else
    gl::Disable(gl::BLEND);
  if (savedState.last_enable_cull_face)
    gl::Enable(gl::CULL_FACE);
  else
    gl::Disable(gl::CULL_FACE);
  if (savedState.last_enable_depth_test)
    gl::Enable(gl::DEPTH_TEST);
  else
    gl::Disable(gl::DEPTH_TEST);
  if (savedState.last_enable_scissor_test)
    gl::Enable(gl::SCISSOR_TEST);
  else
    gl::Disable(gl::SCISSOR_TEST);
  gl::Viewport(savedState.last_viewport[0], savedState.last_viewport[1],
               (gl::GLsizei)savedState.last_viewport[2],
               (gl::GLsizei)savedState.last_viewport[3]);
}

template <typename T> bool is_same_type(std::type_index ti) {
  return ti == std::type_index{typeid(T)};
}

void inputTextString(const char *label, std::string &str, size_t buf_size) {
  std::vector<char> strvec{str.begin(), str.end()};
  strvec.resize(buf_size);
  ImGui::InputText("", strvec.data(), strvec.size());
  str.assign(strvec.begin(), strvec.end());
}

void inputTextMultilineString(const char *label, std::string &str,
                              size_t buf_size) {
  std::vector<char> strvec{str.begin(), str.end()};
  strvec.resize(buf_size);
  auto size = ImGui::GetContentRegionAvail();
  ImGui::InputTextMultiline("", strvec.data(), strvec.size(), size);
  str.assign(strvec.begin(), strvec.end());
}

void enumComboBox(const char *label, int *outValue,
                  span<const std::pair<const char *, int>> values) {
  auto enumToIndex = [&](gl::GLenum e) {
    int i = 0;
    for (auto &p : values) {
      if (p.second == e)
        return i;
      ++i;
    }
    return -1;
  };
  int curIdx = enumToIndex(*outValue);
  ImGui::Combo(label, &curIdx,
               [](void *data, int idx, const char **out_text) {
                 auto values = *static_cast<span<NameValuePair> *>(data);
                 if (idx < (int)values.size()) {
                   *out_text = values[idx].first;
                   return true;
                 }
                 return false;
               },
               &values, (int)values.size());
  if (curIdx != -1)
    *outValue = values[curIdx].second;
}

static const char *getFriendlyName(const meta::Field &f) {
  // if (auto a = f.getAttribute<meta::FriendlyName>()) {
  //  return a->name;
  //} else
  return f.name;
}

static const char *getFriendlyName(const meta::Enumerator &e) {
  // if (auto a = e.getAttribute<meta::FriendlyName>()) {
  //  return a->name;
  //} else
  return e.name;
}

void genericValue(std::type_index ti, void *data) {
  ImGui::PushID(data);
  // GUIs for primitive types
  if (is_same_type<float>(ti)) {
    ImGui::SliderFloat("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (is_same_type<double>(ti)) {
    // ImGui::SliderD("", reinterpret_cast<float*>(data), 0.0f, 10.0f);
  } else if (is_same_type<int>(ti)) {
    ImGui::SliderInt("", reinterpret_cast<int *>(data), 0, 100);
  } else if (is_same_type<bool>(ti)) {
    ImGui::Checkbox("", reinterpret_cast<bool *>(data));
  }
  // GUI for std::string
  else if (is_same_type<std::string>(ti)) {
    auto &str = *reinterpret_cast<std::string *>(data);
    inputTextString("", str);
  }
  // GUIs for GLM vector types
  else if (is_same_type<glm::vec2>(ti)) {
    ImGui::SliderFloat2("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (is_same_type<glm::vec3>(ti)) {
    ImGui::SliderFloat3("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (is_same_type<glm::vec4>(ti)) {
    ImGui::SliderFloat4("", reinterpret_cast<float *>(data), -10.0f, 10.0f);
  } else if (is_same_type<glm::ivec2>(ti)) {
    ImGui::SliderInt2("", reinterpret_cast<int *>(data), 0, 100);
  } else if (is_same_type<glm::ivec3>(ti)) {
    ImGui::SliderInt3("", reinterpret_cast<int *>(data), 0, 100);
  } else if (is_same_type<glm::ivec4>(ti)) {
    ImGui::SliderInt4("", reinterpret_cast<int *>(data), 0, 100);
  }
  // GUIs for reflected enum types
  /*else if (auto mo0 = meta::typeOf(ti)) {
    if (auto mo = mo0->as<meta::Enum>()) {
      int i = mo->findEnumeratorIndex(data);
      auto items_getter = [](void *data, int idx,
                             const char **out_text) -> bool {
        auto mo = static_cast<const meta::Enum *>(data);
        if (idx >= mo->enumerators.size())
          return false;
        *out_text = getFriendlyName(mo->enumerators[idx]);
        return true;
      };
      ImGui::Combo("", &i, items_getter, const_cast<meta::Enum *>(mo),
                   (int)mo->enumerators.size());
      mo->setValue(data, mo->enumerators[i].value);
    }
  }*/ else {
    ImGui::TextDisabled("No metaobject");
  }
  ImGui::PopID();
}
} // namespace gui
} // namespace ag
