// Shared ImGui utilities
#pragma once
#include <autograph/Camera.h>
#include <autograph/engine/Meta.h>
#include <autograph/engine/Light.h>
#include <autograph/engine/RenderableScene.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/engine/Scene.h>
#include <autograph/gl/All.h>

namespace ag {
namespace gui {
void beginFrame();
void sceneEditor(const Camera &cam, EntityManager &entityManager, Scene &scene,
	RenderableScene &renderableScene, LightScene& lights, ResourcePool &resourcePool,
	ID rootEntityID);
void endFrame();

void inputTextString(const char *label, std::string &str,
                     size_t buf_size = 100);

void inputTextMultilineString(const char *label, std::string &str,
                              size_t buf_size = 100);

using NameValuePair = std::pair<const char *, int>;

void enumComboBox(const char *label, int *outValue,
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