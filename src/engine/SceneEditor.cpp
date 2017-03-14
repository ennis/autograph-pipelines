#include <autograph/engine/SceneEditor.h>
#include <autograph/engine/ImGuiUtils.h>
#include <autograph/engine/Light.h>
#include <autograph/engine/Renderable.h>
#include <autograph/engine/ResourcePool.h>
#include <autograph/engine/SceneObject.h>
#include <autograph/engine/SceneLoader.h>
#include <autograph/engine/RenderUtils.h>
#include <autograph/gl/All.h>
#include <autograph/support/FileDialog.h>
#include <autograph/support/ProjectRoot.h>
#include <autograph/engine/imgui.h>
#include <autograph/engine/ImGuizmo.h>

namespace ag {
namespace SceneEditor {

static void editTransform(Transform &tr) {
  vec3 rotEuler = glm::eulerAngles(tr.rotation);
  ImGui::InputFloat3("Tr", &tr.position[0]);
  ImGui::InputFloat3("Rt", &rotEuler[0]);
  ImGui::InputFloat3("Sc", &tr.scaling[0]);
  tr.rotation = quat{rotEuler};
}

static void transformGizmo(const Camera &camera, Transform &tr) {
  mat4 matrix = tr.getMatrix();

  static ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::ROTATE;
  static ImGuizmo::MODE currentGizmoMode = ImGuizmo::WORLD;
  if (ImGui::IsKeyPressed(90))
    currentGizmoOperation = ImGuizmo::TRANSLATE;
  if (ImGui::IsKeyPressed(69))
    currentGizmoOperation = ImGuizmo::ROTATE;
  if (ImGui::IsKeyPressed(82)) // r Key
    currentGizmoOperation = ImGuizmo::SCALE;
  if (ImGui::RadioButton("Translate",
                         currentGizmoOperation == ImGuizmo::TRANSLATE))
    currentGizmoOperation = ImGuizmo::TRANSLATE;
  ImGui::SameLine();
  if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
    currentGizmoOperation = ImGuizmo::ROTATE;
  ImGui::SameLine();
  if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
    currentGizmoOperation = ImGuizmo::SCALE;

  if (currentGizmoOperation != ImGuizmo::SCALE) {
    if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
      currentGizmoMode = ImGuizmo::LOCAL;
    ImGui::SameLine();
    if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
      currentGizmoMode = ImGuizmo::WORLD;
  }
  static bool useSnap(false);
  if (ImGui::IsKeyPressed(83))
    useSnap = !useSnap;
  ImGui::Checkbox("", &useSnap);
  ImGui::SameLine();
  vec3 snap{0.0f};
  /*vec3 snap;
  switch (currentGizmoOperation) {
  case ImGuizmo::TRANSLATE:
    snap = config.mSnapTranslation;
    ImGui::InputFloat3("Snap", &snap[0]);
    break;
  case ImGuizmo::ROTATE:
    snap = config.mSnapRotation;
    ImGui::InputFloat("Angle Snap", &snap[0]);
    break;
  case ImGuizmo::SCALE:
    snap = config.mSnapScale;
    ImGui::InputFloat("Scale Snap", &snap[0]);
    break;
  }*/

  ImGuizmo::Manipulate(&camera.viewMat[0][0], &camera.projMat[0][0],
                       currentGizmoOperation, currentGizmoMode, &matrix[0][0],
                       nullptr, useSnap ? &snap[0] : nullptr);
  // convert matrix back to transform components
  tr = Transform::fromMatrix(matrix);
}

static void sceneObjectHierarchyGui(SceneObjectComponents &scene, 
		  Scene& genericScene,
		  SceneObject &sceneObj,
                                    ID &selectedID) {
  const char *node_name = "<no name>";
  if (!sceneObj.name.empty())
    node_name = sceneObj.name.c_str();
  ImGui::PushID(&sceneObj);
  ImGuiTreeNodeFlags node_flags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
  if (sceneObj.entityID == selectedID)
    node_flags |= ImGuiTreeNodeFlags_Selected;
  bool opened = false;
  if (sceneObj.children.empty()) {
    ImGui::TreeNodeEx(&sceneObj, node_flags | ImGuiTreeNodeFlags_Leaf |
                                     ImGuiTreeNodeFlags_NoTreePushOnOpen,
                      "%s", node_name);
    if (ImGui::IsItemClicked())
      selectedID = sceneObj.entityID;
  } else {
    opened = ImGui::TreeNodeEx(&sceneObj, node_flags, "%s", node_name);
    if (ImGui::IsItemClicked())
      selectedID = sceneObj.entityID;
  }
  if (opened) {
    for (auto c : sceneObj.children) {
      sceneObjectHierarchyGui(scene, genericScene, *c, selectedID);
    }
    ImGui::TreePop();
  }
  ImGui::PopID();
}

void show(const Camera &cam, EntityManager &entityManager, SceneObjectComponents &scene,
		  Scene& genericScene,
          RenderableComponents &renderableScene, LightComponents &lights,
          ResourcePool &resourcePool) {

  bool opened = true;
  ImGui::Begin("Scene", &opened, ImGuiWindowFlags_MenuBar);

  ImGui::BeginMenuBar();
  if (ImGui::Button("Load scene...")) {
    auto res = openFileDialog("", getProjectRootDirectory().c_str());
    if (res) {
      ID ent = loadScene(res->c_str(), entityManager, scene, renderableScene,
                         lights, resourcePool);
    }
  }
  ImGui::EndMenuBar();

  static ID selectedID = 0;

  ImGui::BeginChild("Hierarchy",
                    ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f,
                           ImGui::GetContentRegionAvail().y),
                    true, ImGuiWindowFlags_HorizontalScrollbar);

  for (auto &obj : scene.getObjects()) {
    if (!obj.second.parent) {
      sceneObjectHierarchyGui(scene, genericScene, obj.second, selectedID);
    }
  }

  ImGui::EndChild();

  SceneObject *selected = scene.get(selectedID);
  if (selected) {
    ImGui::SameLine();
    ImGui::BeginChild("Components", ImVec2(0, 300), false);
    //if (ImGui::CollapsingHeader("Transform")) {
    //  editTransform(selected->localTransform);
    //}
    transformGizmo(cam, selected->localTransform);
    // individual component GUIs
    genericScene.showGUI(selectedID);
    ImGui::EndChild();

    if (selected->mesh) {
      RenderUtils::drawWireMesh(gl::getDefaultFramebuffer(), cam,
                                *selected->mesh, selected->worldTransform);
    }
    RenderUtils::drawBoundingBox(gl::getDefaultFramebuffer(), cam,
                                 selected->getApproximateWorldBounds());
  }

  ImGui::End();
}
}
}