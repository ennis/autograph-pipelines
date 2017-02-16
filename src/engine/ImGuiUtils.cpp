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

static void editTransform(const Camera &camera, Transform &tr) {
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

  float matrixTranslation[3], matrixRotation[3], matrixScale[3];
  ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], matrixTranslation,
                                        matrixRotation, matrixScale);
  ImGui::InputFloat3("Tr", matrixTranslation, 3);
  ImGui::InputFloat3("Rt", matrixRotation, 3);
  ImGui::InputFloat3("Sc", matrixScale, 3);
  ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation,
                                          matrixScale, &matrix[0][0]);

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
  ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], matrixTranslation,
                                        matrixRotation, matrixScale);
  tr = Transform::fromMatrix(matrix);
  /*tr.position.x = matrixTranslation[0];
  tr.position.y = matrixTranslation[1];
  tr.position.z = matrixTranslation[2];
  tr.rotation = quat{matrix};   // extract rotation component from matrix
  //tr.rotation = quat{vec3{matrixRotation[0], matrixRotation[1], matrixRotation[2]}};
  tr.scaling.x = matrixScale[0];
  tr.scaling.y = matrixScale[1];
  tr.scaling.z = matrixScale[2];*/
}

void sceneEditor(const Camera &cam, EntityList &scene, ID rootEntityID) {
  ImGui::Begin("Scene editor");
  Entity *rootEntity = scene.get(rootEntityID);
  SceneObject *rootSceneObject = rootEntity->getComponent<SceneObject>();
  editTransform(cam, rootSceneObject->localTransform);
  ImGui::End();
}

}
}
