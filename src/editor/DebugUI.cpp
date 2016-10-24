/*#include "Meta.hpp"
#include "RectTransform.hpp"
#include "Scene.hpp"
#include <cppformat/format.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>*/

/*static Entity g_selected_entity = nullptr;

template <typename T> bool is_same_type(std::type_index ti) {
  return ti == std::type_index{typeid(T)};
}

void imgui_input_string(const char *label, std::string &str,
                        size_t buf_size = 100) {
  std::vector<char> strvec{str.begin(), str.end()};
  strvec.resize(buf_size);
  ImGui::InputText("", strvec.data(), strvec.size());
  str.assign(strvec.begin(), strvec.end());
}

const char *get_friendly_name(const meta::Field &f) {
  if (auto a = f.getAttribute<meta::FriendlyName>()) {
    return a->name;
  } else
    return f.name;
}

const char *get_friendly_name(const meta::Enumerator &e) {
  if (auto a = e.getAttribute<meta::FriendlyName>()) {
    return a->name;
  } else
    return e.name;
}

void value_debug_gui(std::type_index ti, void *data) {
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
    imgui_input_string("", str);
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
  else if (auto mo0 = meta::typeOf(ti)) {
    if (auto mo = mo0->as<meta::Enum>()) {
      int i = mo->findEnumeratorIndex(data);
      auto items_getter = [](void *data, int idx,
                             const char **out_text) -> bool {
        auto mo = static_cast<const meta::Enum*>(data);
        if (idx >= mo->enumerators.size())
          return false;
        *out_text = get_friendly_name(mo->enumerators[idx]);
        return true;
      };
      ImGui::Combo("", &i, items_getter, const_cast<meta::Enum *>(mo),
                   mo->enumerators.size());
      mo->setValue(data, mo->enumerators[i].value);
    }
  } else {
    ImGui::TextDisabled("No metaobject");
  }
  ImGui::PopID();
}

void entity_debug_gui(Entity *e) {
  ImGui::PushID(e);
  if (auto rt = e->getComponent<RectTransform>()) {
    if (rt->children.empty()) {
      if (ImGui::Selectable(e->name.c_str(), g_selected_entity.get() == e)) {
        g_selected_entity = e;
      }
    } else {
      bool opened = ImGui::TreeNode("##nolabel");
      ImGui::SameLine();
      if (ImGui::Selectable(e->name.c_str(), g_selected_entity.get() == e))
        g_selected_entity = e;
      if (opened) {
        for (auto &&child : rt->children)
          entity_debug_gui(child.get());
        ImGui::TreePop();
      }
    }
  } else {
    ImGui::Text("%s", e->name.c_str());
  }
  ImGui::PopID();
}

void component_debug_gui(std::type_index ti, void *data) {
  auto metaobj0 = meta::typeOf(ti);
  if (!metaobj0) {
    ImGui::CollapsingHeader("Unknown (no metaobject)");
    return;
  }
  auto metaobj = metaobj0->as<meta::Record>();
  if (!metaobj) {
    ImGui::CollapsingHeader("Unknown (not a struct)");
    return;
  }

  if (ImGui::CollapsingHeader(metaobj->name)) {
    ImGui::Columns(2);
    for (auto &&f : metaobj->publicFields) {
      ImGui::Text("%s", get_friendly_name(f));
      ImGui::NextColumn();
      ImGui::PushItemWidth(-1.0f);
      value_debug_gui(f.typeindex, f.getPtr(data));
      ImGui::NextColumn();
    }
    ImGui::Columns(1);
  }
}

void scene_debug_gui(Scene &s) {
  for (auto &&e : s.getEntities()) {
    // skip non-root entities
    if (auto rt = e->getComponent<RectTransform>())
      if (!rt->parent)
        entity_debug_gui(e.get());
  }
  ImGui::Separator();
  ImGui::Text("Selected entity: %p\n", g_selected_entity.get());
  if (g_selected_entity) {
    imgui_input_string("Name", g_selected_entity->name);
    ImGui::Separator();
    if (auto rt = g_selected_entity->getComponent<RectTransform>()) {
      component_debug_gui(std::type_index{typeid(RectTransform)}, rt);
    }
    if (auto vis = g_selected_entity->getComponent<ui::Visual>()) {
      component_debug_gui(typeid(*vis), vis);
    }
    if (auto layout =
            g_selected_entity->getComponent<ui::LayoutController>()) {
      component_debug_gui(typeid(*layout), layout);
    }
  }
}
*/