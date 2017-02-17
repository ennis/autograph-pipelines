#include <autograph/Camera.h>
#include <autograph/Transform.h>
#include <autograph/engine/ImGuiUtils.h>
#include <autograph/engine/Scene.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <glm/gtx/quaternion.hpp>

namespace ag {
namespace gui {
void beginFrame() {
    ImGuizmo::BeginFrame();
}

void endFrame() {
}

static void editTransform(Transform &tr)
{
	vec3 rotEuler = glm::eulerAngles(tr.rotation);
	ImGui::InputFloat3("Tr", &tr.position[0]);
	ImGui::InputFloat3("Rt", &rotEuler[0]);
	ImGui::InputFloat3("Sc", &tr.scaling[0]);
	tr.rotation = quat{ rotEuler };
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

  auto oldpos = tr.position;

  ImGuizmo::Manipulate(&camera.viewMat[0][0], &camera.projMat[0][0],
                       currentGizmoOperation, ImGuizmo::LOCAL, &matrix[0][0],
                       nullptr, useSnap ? &snap[0] : nullptr);

  // convert matrix back to transform components
  tr = Transform::fromMatrix(matrix);
}

static void sceneObjectGui(EntityList &scene, SceneObject *sceneObj) {
	Entity* ent = scene.get(sceneObj->id);
	ImGui::PushID(sceneObj->id);
  if (ImGui::TreeNode(ent->getName().c_str())) {
	  editTransform(sceneObj->localTransform);
    for (auto c : sceneObj->children) {
      sceneObjectGui(scene, c);
    }
    ImGui::TreePop();
  }
  ImGui::PopID();
}

void sceneEditor(const Camera &cam, EntityList &scene, ID rootEntityID) {
  ImGui::Begin("Scene editor");
  Entity *rootEntity = scene.get(rootEntityID);
  SceneObject *rootSceneObject = rootEntity->getComponent<SceneObject>();
  editTransform(rootSceneObject->localTransform);
  transformGizmo(cam, rootSceneObject->localTransform);
  sceneObjectGui(scene, rootSceneObject);
  ImGui::End();
}

}
}
