#include <SceneEditor/Exports.h>
#include <autograph/Editor/SceneEditor.h>
#include <autograph/Engine/All.h>
#include <autograph/Gfx/All.h>
#include <autograph/Core/Transform.h>
#include <autograph/Core/Support/FileDialog.h>
#include <autograph/Core/Support/ProjectRoot.h>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace ag {
static void editTransform(Transform &tr) {
  vec3 rotEuler = glm::eulerAngles(tr.rotation);
  ImGui::InputFloat3("Tr", &tr.position[0]);
  ImGui::InputFloat3("Rt", &rotEuler[0]);
  ImGui::InputFloat3("Sc", &tr.scaling[0]);
  tr.rotation = quat{rotEuler};
}

class SparseTexture2D {
public:
  SparseTexture2D() {}

  SparseTexture2D(ImageFormat format, int width, int height, int numMips) {
    gl::GLuint tex;
    gl::CreateTextures(gl::TEXTURE_2D, 1, &tex);
    // set tex parameters
    gl::TextureParameteri(tex, gl::TEXTURE_SWIZZLE_R, gl::RED);
    gl::TextureParameteri(tex, gl::TEXTURE_SWIZZLE_G, gl::GREEN);
    gl::TextureParameteri(tex, gl::TEXTURE_SWIZZLE_B, gl::BLUE);
    gl::TextureParameteri(tex, gl::TEXTURE_SWIZZLE_A, gl::ALPHA);
    gl::TextureParameteri(tex, gl::TEXTURE_BASE_LEVEL, 0);
    gl::TextureParameteri(tex, gl::TEXTURE_MIN_FILTER,
                          gl::LINEAR_MIPMAP_NEAREST);
    gl::TextureParameteri(tex, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
    gl::TextureParameteri(tex, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
    gl::TextureParameteri(tex, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
    gl::TextureParameteri(tex, gl::TEXTURE_SPARSE_ARB, gl::TRUE_);
    // Allocate virtual storage
    auto glfmt = getGLImageFormatInfo(format);
    gl::TextureStorage2D(tex, numMips, glfmt.internal_fmt, width, height);
    // Get page size
    gl::GetInternalformativ(gl::TEXTURE_2D, glfmt.internal_fmt,
                            gl::VIRTUAL_PAGE_SIZE_X_ARB, 1, &pageSizeX);
    gl::GetInternalformativ(gl::TEXTURE_2D, glfmt.internal_fmt,
                            gl::VIRTUAL_PAGE_SIZE_Y_ARB, 1, &pageSizeY);
    gl::GetInternalformativ(gl::TEXTURE_2D, glfmt.internal_fmt,
                            gl::VIRTUAL_PAGE_SIZE_Z_ARB, 1, &pageSizeZ);
    AG_DEBUG("SparseTexture: page size ({}) = {}x{}x{}",
             getImageFormatInfo(format).name, pageSizeX, pageSizeY, pageSizeZ);
  }

  void commitRegion(int x, int y, int width, int height) {}

private:
  int pageSizeX = 0;
  int pageSizeY = 0;
  int pageSizeZ = 0;
  GLHandle<TextureDeleter> obj_;
};

/////////////////////////////////////////////////////////////////////////////////////
// Transform gizmo
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
  // ImGui::SameLine();
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

enum class InterpolationMode { Constant, Linear, Cubic };

enum class RepeatMode { Repeat, Once };

struct CurvePoint {
  InterpolationMode interpolationMode;
  float time;
  float value;
  float slopeBefore;
  float slopeAfter;
};

float interpolateCurve(const CurvePoint &A, const CurvePoint &B, float time) {
  // hermite interpolation
  auto p0 = A.value;
  auto x0 = A.time;
  auto t0 = A.slopeAfter;
  auto p1 = B.value;
  auto x1 = B.time;
  auto t1 = B.slopeBefore;
  auto dx = x1 - x0;
  auto tt = (time - x0) / dx;
  auto tt2 = tt * tt;
  auto tt3 = tt2 * tt;
  auto h00 = 2 * tt3 - 3 * tt2 + 1;
  auto h10 = tt3 - 2 * tt2 + tt;
  auto h01 = -2 * tt3 + 3 * tt2;
  auto h11 = tt3 - tt2;
  return h00 * p0 + h10 * dx * t0 + h01 * p1 + h11 * dx * t1;
}

auto findNearestCurvePoints(span<const CurvePoint> curve, float time,
                            RepeatMode repeatMode) {
  // dichotomy
  int l = 0;
  int u = (int)curve.size();
  int p = l + (u - l) / 2;
  while (p != l) {
    if (time < curve[p].time) {
      u = p;
    } else if (time > curve[p].time) {
      l = p;
    } else {
      break;
    }
    p = l + (u - l) / 2;
  }
  // final check
  int first = p;
  int second = std::min(p + 1, (int)(curve.size() - 1));
  if (time < curve[p].time) {
    return std::make_pair(-1, 0);
  } else if (time > curve[second].time) {
    return std::make_pair(second, second + 1);
  }
  return std::make_pair(first, second);
};

float evaluateCurve(span<const CurvePoint> curve, float time,
                    RepeatMode repeatMode) {
  // hermite interpolation
  auto pair = findNearestCurvePoints(curve, time, repeatMode);
  if (pair.first < 0)
    pair.first = 0;
  if (pair.second >= (int)curve.size())
    pair.second = (int)curve.size() - 1;
  return interpolateCurve(curve[pair.first], curve[pair.second], time);
}

/////////////////////////////////////////////////////////////////////////////////////
// Texture viewer
// if disp{Height,Width} = -1, fill all available space
// The texture is never stretched
/*static void textureViewer(const ag::Texture &tex, float widgetWidth,
                          float widgetHeight) {
  static Shader textureViewShader{"shaders/default:textureView"};
  static SamplerDesc textureViewSamplerDesc{
      gl::CLAMP_TO_EDGE, gl::CLAMP_TO_EDGE, gl::CLAMP_TO_EDGE, gl::NEAREST,
      gl::NEAREST};
  static Sampler textureViewSampler{textureViewSamplerDesc};

  int width = tex.width();
  int height = tex.height();

  static int xoffset = 0;
  static int yoffset = 0;
  static float zoomLevel = 1;
  static bool mirror = false;
  static bool autofit = true;

  if (autofit) {
    autofit = false;
  }

  ImVec2 contentRegion = ImGui::GetContentRegionAvail();
  float dispWidth = widgetWidth < 0 ? contentRegion.x : widgetWidth;
  float dispHeight = widgetHeight < 0 ? contentRegion.y : widgetHeight;
  ImGui::Dummy(ImVec2{dispWidth, dispHeight});

  auto pos = ImGui::GetItemRectMin();
  auto size = ImGui::GetItemRectSize();

  auto screenToUV = [&](vec2 screen) {

  };

  gui::customRendering([=](const ImDrawList *parentList, const ImDrawCmd *cmd) {
    auto &fb = getDefaultFramebuffer();
    auto fb_width = fb.width();
    auto fb_height = fb.height();
    float uv_l = (float)xoffset / (float)w;
    float uv_t = 1.0f / zoomLevel;
    float uv_r = 1.0f / zoomLevel;
    float uv_b = (float)yoffset / (float)h;
    if (mirror)
      std::swap(uv_t, uv_b);
    ag::drawRect(getDefaultFramebuffer(), pos.x, pos.y, pos.x + size.x,
                 pos.y + size.y, uv_l, uv_t, uv_r, uv_b,
                 getDebugGlobals().textureViewShader,
                 bind::scissor(0, (int)cmd->ClipRect.x,
                               (int)(fb_height - cmd->ClipRect.w),
                               (int)(cmd->ClipRect.z - cmd->ClipRect.x),
                               (int)(cmd->ClipRect.w - cmd->ClipRect.y)),
                 bind::texture(0, textureObj,
                               getDebugGlobals().textureViewSampler.object()),
                 bind::uniform_float("uLod", lod),
                 bind::uniform_vec2("uRange", vec2(range_min, range_max)),
                 bind::uniform_vec4("uBorder", vec4(0.1f, 0.1f, 0.1f, 1.0f)));
  });
}
*/
static void curveEditor(std::vector<CurvePoint> &curve, float time) {
  using ag::gui::toVec2;
  using ag::gui::toImVec2;
  /////////////////////////////////////////////////////////////////
  // state
  const float pointHitTestRadius = 4.0f;
  const float tangentsPixelSize = 50.0f;
  const float height = 300.0f;
  static int selectedPointIndex = -1;
  static bool dragTangentHandleA = false;
  static bool dragTangentHandleB = false;
  static bool draggingPoint = false;

  if (curve.empty())
    return;
  /////////////////////////////////////////////////////////////////
  // check that the curve is sorted
  float prevTime = curve[0].time;
  for (auto &p : curve) {
    if (prevTime > p.time) {
      errorMessage("Curve points are not sorted");
      return;
    }
    prevTime = p.time;
  }

  /////////////////////////////////////////////////////////////////
  // display window
  static bool autofit = true;
  static struct DisplayWindow {
    vec2 curveMin;
    vec2 curveMax;
    vec2 origCurveMin;
    vec2 origCurveMax;
    vec2 rectMin;
    vec2 rectMax;
    bool panning;
  } dwin;

  /////////////////////////////////////////////////////////////////
  // Helpers
  auto screenToCurve = [&](float x, float y) {
    auto dispX = (x - dwin.rectMin.x) / (dwin.rectMax.x - dwin.rectMin.x);
    auto dispY = (y - dwin.rectMin.y) / (dwin.rectMax.y - dwin.rectMin.y);
    auto time = glm::mix(dwin.curveMin.x, dwin.curveMax.x, dispX);
    auto val = glm::mix(dwin.curveMin.y, dwin.curveMax.y, dispY);
    return vec2{time, val};
  };

  auto screenVectorToCurve = [&](float x, float y) {
    auto dispX = x / (dwin.rectMax.x - dwin.rectMin.x);
    auto dispY = y / (dwin.rectMax.y - dwin.rectMin.y);
    auto time = dispX * (dwin.curveMax.x - dwin.curveMin.x);
    auto val = dispY * (dwin.curveMax.y - dwin.curveMin.y);
    return vec2{time, val};
  };

  auto curveToScreen = [&](float time, float val) {
    auto dispX = (time - dwin.curveMin.x) / (dwin.curveMax.x - dwin.curveMin.x);
    auto dispY = (val - dwin.curveMin.y) / (dwin.curveMax.y - dwin.curveMin.y);
    auto x = glm::mix(dwin.rectMin.x, dwin.rectMax.x, dispX);
    auto y = glm::mix(dwin.rectMin.y, dwin.rectMax.y, dispY);
    return vec2{x, y};
  };

  auto curveVectorToScreen = [&](vec2 v) {
    auto dispX = v.x / (dwin.curveMax.x - dwin.curveMin.x);
    auto dispY = v.y / (dwin.curveMax.y - dwin.curveMin.y);
    auto x = dispX * (dwin.rectMax.x - dwin.rectMin.x);
    auto y = dispY * (dwin.rectMax.y - dwin.rectMin.y);
    return vec2{x, y};
  };

  /////////////////////////////////////////////////////////////////
  // auto-fit display window
  if (autofit) {
    dwin.curveMin.x = curve[0].time;
    dwin.curveMax.x = curve[curve.size() - 1].time;
    dwin.curveMin.y = 0.0f;
    dwin.curveMax.y = 100.0f;
    autofit = false;
  }

  const float availWidth = ImGui::GetContentRegionAvailWidth();
  ImGui::BeginChild("stuff");
  ImGui::Dummy(ImVec2{availWidth, height});
  ImGui::EndChild();
  bool hovered = ImGui::IsItemHovered();
  bool clicked = ImGui::IsItemClicked();
  bool rightclicked = ImGui::IsItemClicked(1);
  bool hitTestPoint = false;
  auto rectMin = ImGui::GetItemRectMin();
  auto rectMax = ImGui::GetItemRectMax();
  dwin.rectMin.x = rectMin.x;
  dwin.rectMax.x = rectMax.x;
  dwin.rectMin.y = rectMin.y;
  dwin.rectMax.y = rectMax.y;

  auto drawList = ImGui::GetWindowDrawList();

  std::vector<ImVec2> evalPoints;
  const int nbEvalPoints = std::max(1, (int)(availWidth / 2));
  evalPoints.resize(nbEvalPoints);

  /////////////////////////////////////////////////////////////////
  // Background grid
  // TODO

  /////////////////////////////////////////////////////////////////
  // Draw curve
  for (int i = 0; i < nbEvalPoints; ++i) {
    float f = (float)i / (nbEvalPoints - 1);
    float tcur = glm::mix(dwin.curveMin.x, dwin.curveMax.x, f);
    evalPoints[i] = toImVec2(
        curveToScreen(tcur, evaluateCurve(curve, tcur, RepeatMode::Once)));
  }
  drawList->AddPolyline(evalPoints.data(), nbEvalPoints,
                        IM_COL32(0, 255, 0, 255), false, 1.0f, true);

  /////////////////////////////////////////////////////////////////
  // draw points
  for (int i = 0; i < curve.size(); ++i) {
    auto vpos = curveToScreen(curve[i].time, curve[i].value);
    ImVec2 pos{vpos.x, vpos.y};
    drawList->AddCircleFilled(pos, pointHitTestRadius,
                              IM_COL32(128, 255, 128, 255));
  }

  vec2 mousePos = toVec2(ImGui::GetMousePos());
  vec2 curvePos = screenToCurve(mousePos.x, mousePos.y);

  /////////////////////////////////////////////////////////////////
  // Limits
  auto lbl_vmin = fmt::format("{}", dwin.curveMin.y);
  auto lbl_vmax = fmt::format("{}", dwin.curveMax.y);
  auto lbl_tmin = fmt::format("{}", dwin.curveMin.x);
  auto lbl_tmax = fmt::format("{}", dwin.curveMax.x);

  enum class AlignCorner {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
  };

  auto alignText = [&](const char *text, ImVec2 pos, AlignCorner corner) {
    const ImVec2 textSize = ImGui::CalcTextSize(text);
    switch (corner) {
    // Top
    default:
    case AlignCorner::TopLeft:
      return pos;
    case AlignCorner::TopCenter:
      return ImVec2{pos.x - textSize.x / 2, pos.y};
    case AlignCorner::TopRight:
      return ImVec2{pos.x - textSize.x, pos.y};
    // Center
    case AlignCorner::CenterLeft:
      return ImVec2{pos.x, pos.y - textSize.y / 2};
    case AlignCorner::Center:
      return ImVec2{pos.x - textSize.x / 2, pos.y - textSize.y / 2};
    case AlignCorner::CenterRight:
      return ImVec2{pos.x - textSize.x, pos.y - textSize.y / 2};
    // Bottom right
    case AlignCorner::BottomLeft:
      return ImVec2{pos.x, pos.y - textSize.y};
    case AlignCorner::BottomCenter:
      return ImVec2{pos.x - textSize.x / 2, pos.y - textSize.y};
    case AlignCorner::BottomRight:
      return ImVec2{pos.x - textSize.x, pos.y - textSize.y};
    }
  };

  auto addTextAlign = [&](const char *text, ImVec2 pos, AlignCorner corner) {
    drawList->AddText(alignText(text, pos, corner),
                      IM_COL32(255, 255, 255, 255), text);
  };

  addTextAlign(lbl_vmin.c_str(), ImVec2{dwin.rectMin.x, dwin.rectMin.y},
               AlignCorner::TopLeft);
  addTextAlign(lbl_vmax.c_str(), ImVec2{dwin.rectMin.x, dwin.rectMax.y},
               AlignCorner::BottomLeft);

  /////////////////////////////////////////////////////////////////
  // Select control point
  for (int i = 0; i < curve.size(); ++i) {
    auto pos = curveToScreen(curve[i].time, curve[i].value);
    auto vpos = vec2(pos.x, pos.y);
    if (glm::distance(vpos, mousePos) < pointHitTestRadius) {
      drawList->AddCircleFilled(toImVec2(pos), pointHitTestRadius,
                                IM_COL32(255, 0, 0, 255));
      if (clicked && !hitTestPoint) {
        selectedPointIndex = i;
        hitTestPoint = true;
        draggingPoint = true; // entry condition for drag
      }
    }
  }

  /////////////////////////////////////////////////////////////////
  // Drag control point
  if (ImGui::IsMouseDragging(0, 1.0f) && selectedPointIndex != -1 &&
      draggingPoint == true) {
    hitTestPoint = true;
    curve[selectedPointIndex].time = curvePos.x;
    if (selectedPointIndex > 0) {
      auto prevPoint = curve[selectedPointIndex - 1];
      curve[selectedPointIndex].time =
          std::max(prevPoint.time, curve[selectedPointIndex].time);
    }
    if (selectedPointIndex < (int)curve.size() - 1) {
      auto nextPoint = curve[selectedPointIndex + 1];
      curve[selectedPointIndex].time =
          std::min(nextPoint.time, curve[selectedPointIndex].time);
    }
    curve[selectedPointIndex].value =
        glm::clamp(curvePos.y, dwin.curveMin.y, dwin.curveMax.y);
  }
  if (!ImGui::IsMouseDown(0)) { // exit condition for drag
    draggingPoint = false;
  }

  /////////////////////////////////////////////////////////////////
  // View panning
  if (rightclicked) { // entry condition
    // dwin.panning = true;
    dwin.origCurveMin = dwin.curveMin;
    dwin.origCurveMax = dwin.curveMax;
  }
  if (ImGui::IsMouseDragging(1, 0.0f)) {
    auto delta = ImGui::GetMouseDragDelta(1, 0.0f);
    auto deltaCurve = screenVectorToCurve(delta.x, delta.y);
    auto vdelta = -vec2{deltaCurve.x, deltaCurve.y};
    dwin.curveMin = dwin.origCurveMin + vdelta;
    dwin.curveMax = dwin.origCurveMax + vdelta;
    hitTestPoint = true;
  }
  if (!ImGui::IsMouseDown(1)) { // exit condition
    // dwin.panning = false;
  }

  /////////////////////////////////////////////////////////////////
  // Selected point & tangents
  if (selectedPointIndex != -1) {
    auto &curvePoint = curve[selectedPointIndex];
    vec2 pos = curveToScreen(curvePoint.time, curvePoint.value);
    drawList->AddCircleFilled(toImVec2(pos), pointHitTestRadius,
                              IM_COL32(255, 0, 0, 255));
    // tangents
    vec2 dxB =
        glm::normalize(curveVectorToScreen(vec2{1.0, curvePoint.slopeBefore}));
    vec2 dxA =
        glm::normalize(curveVectorToScreen(vec2{1.0, curvePoint.slopeAfter}));
    vec2 posA = pos + tangentsPixelSize * dxA;
    vec2 posB = pos - tangentsPixelSize * dxB;
    drawList->AddLine(toImVec2(pos), toImVec2(posA), IM_COL32(255, 0, 0, 255));
    drawList->AddLine(toImVec2(pos), toImVec2(posB), IM_COL32(255, 0, 0, 255));
    // tangent controls
    drawList->AddCircleFilled(toImVec2(posA), 4.0f, IM_COL32(255, 0, 0, 255));
    drawList->AddCircleFilled(toImVec2(posB), 4.0f, IM_COL32(255, 0, 0, 255));
    // tangent control hit-test
    if (clicked && !hitTestPoint &&
        glm::distance(posA, mousePos) < pointHitTestRadius) {
      hitTestPoint = true;
      dragTangentHandleA = true;
    } else if (clicked && !hitTestPoint &&
               glm::distance(posB, mousePos) < pointHitTestRadius) {
      hitTestPoint = true;
      dragTangentHandleB = true;
    }
    if (!ImGui::IsMouseDown(0)) {
      dragTangentHandleA = false;
      dragTangentHandleB = false;
    }
    if (ImGui::IsMouseDragging(0, 0.0f) && dragTangentHandleA) {
      vec2 screenTangent = mousePos - pos;
      vec2 curveTangent = screenVectorToCurve(screenTangent.x, screenTangent.y);
      curvePoint.slopeAfter = curveTangent.y / curveTangent.x;
      hitTestPoint = true;
    }
    if (ImGui::IsMouseDragging(0, 0.0f) && dragTangentHandleB) {
      vec2 screenTangent = mousePos - pos;
      vec2 curveTangent = screenVectorToCurve(screenTangent.x, screenTangent.y);
      curvePoint.slopeBefore = curveTangent.y / curveTangent.x;
      hitTestPoint = true;
    }
  }

  /////////////////////////////////////////////////////////////////
  // Zoom
  float scroll = ImGui::GetIO().MouseWheel;
  if (scroll) {
    const float scale = std::pow(2.0f, scroll);
    const vec2 vPos{curvePos.x, curvePos.y};
    vec2 rMin = dwin.curveMin - vPos;
    vec2 rMax = dwin.curveMax - vPos;
    rMin *= scale;
    rMax *= scale;
    rMin += vPos;
    rMax += vPos;
    dwin.curveMin = rMin;
    dwin.curveMax = rMax;
  }

  /////////////////////////////////////////////////////////////////
  // Create point
  if (clicked && !hitTestPoint) {
    // clicked somewhere in the void
    selectedPointIndex = -1;
    // float t = curvePos.x;
    // float val = evaluateCurve(curve, curvePos.x, RepeatMode::Once);
    auto nearestPoints =
        findNearestCurvePoints(curve, curvePos.x, RepeatMode::Once);
    AG_DEBUG("Nearest points: {},{}", nearestPoints.first,
             nearestPoints.second);
    // insert new point
    curve.insert(curve.begin() + nearestPoints.second,
                 CurvePoint{InterpolationMode::Cubic, curvePos.x, curvePos.y,
                            0.0f, 0.0f});
    selectedPointIndex = nearestPoints.second;
    draggingPoint = true;
  }

  /////////////////////////////////////////////////////////////////
  // Guides
  if (!hitTestPoint && hovered) {
    float t = curvePos.x;
    float val = evaluateCurve(curve, curvePos.x, RepeatMode::Once);
    drawList->AddLine(ImVec2{mousePos.x, dwin.rectMin.y},
                      ImVec2{mousePos.x, dwin.rectMax.y},
                      IM_COL32(50, 50, 50, 255));
    vec2 circlePos = curveToScreen(
        curvePos.x, val);
    drawList->AddCircleFilled(toImVec2(circlePos), pointHitTestRadius,
                              IM_COL32(128, 255, 128, 255));
    ImGui::SetTooltip("f(%f) = %f", t, val);
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Scene Editor
class DefaultSceneEditor : public SceneEditor {
public:
  void sceneObjectHierarchyGui(SceneObjectComponents &scene,
                               Scene &genericScene, SceneObject &sceneObj,
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
      ImGui::TreeNodeEx(&sceneObj,
                        node_flags | ImGuiTreeNodeFlags_Leaf |
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

  virtual void onSceneEditorGUI(Scene &s, ID &selectedID, const Camera &camera,
                                ResourcePool &resourcePool) override {
    EntityManager &entityManager = s.getEntityManager();
    SceneObjectComponents *sceneObjects =
        s.getComponentManager<SceneObjectComponents>();
    if (!sceneObjects)
      return;

    bool opened = true;
    ImGui::Begin("Scene", &opened, ImGuiWindowFlags_MenuBar);
    ImGui::BeginMenuBar();
    ImGui::Button("Load scene...");
    ImGui::EndMenuBar();
    ImGui::BeginChild("Hierarchy",
                      ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f,
                             ImGui::GetContentRegionAvail().y),
                      true, ImGuiWindowFlags_HorizontalScrollbar);

    for (auto &obj : sceneObjects->getObjects()) {
      if (!obj.second.parent) {
        sceneObjectHierarchyGui(*sceneObjects, s, obj.second, selectedID);
      }
    }

    ImGui::EndChild();

    static std::vector<CurvePoint> curve{
        {InterpolationMode::Cubic, 0.0f, 0.5f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 0.2f, 0.2f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 0.3f, 0.0f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 0.5f, 1.5f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 0.6f, 2.5f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 0.7f, 1.3f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 0.75f, 1.1f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 0.8f, 0.8f, 0.0f, 0.0f},
        {InterpolationMode::Cubic, 1.0f, 0.7f, 0.0f, 0.0f}};

    SceneObject *selected = sceneObjects->get(selectedID);
    if (selected) {
      ImGui::SameLine();
      ImGui::BeginChild("Components", ImVec2(0, 300), false);
      // if (ImGui::CollapsingHeader("Transform")) {
      //  editTransform(selected->localTransform);
      //}
      transformGizmo(camera, selected->localTransform);
      // individual component GUIs
      s.showGUI(selectedID);
      ImGui::EndChild();

      if (selected->mesh) {
        RenderUtils::drawWireMesh(getGfxContext().getDefaultFramebuffer(), camera,
                                  *selected->mesh, selected->worldTransform);
      }
      RenderUtils::drawBoundingBox(getGfxContext().getDefaultFramebuffer(), camera,
                                   selected->getApproximateWorldBounds());
    }

    ImGui::End();

    ImGui::Begin("Curve editor");
    curveEditor(curve, 0.5f);
    ImGui::End();
  }
};
} // namespace ag

SceneEditor_PLUGIN_ENTRY {
  ag::registerClass<ag::DefaultSceneEditor, ag::SceneEditor>("SceneEditor");
}
