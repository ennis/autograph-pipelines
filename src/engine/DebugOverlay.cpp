#include <autograph/engine/CVar.h>
#include <autograph/engine/DebugOverlay.h>
#include <autograph/engine/ImageUtils.h>
#include <autograph/engine/ImGuiUtils.h>
#include <autograph/engine/RenderUtils.h>
#include <autograph/engine/Shader.h>
#include <autograph/gl/Device.h>
#include <autograph/gl/Draw.h>
#include <autograph/gl/GLHandle.h>
#include <autograph/gl/Sampler.h>
#include <autograph/support/Debug.h>
#include <autograph/support/FileDialog.h>
#include <autograph/support/ProjectRoot.h>
#include <cinttypes>
#include <imgui.h>
#include <imgui_internal.h>

namespace ag {

static const char *imageFileFilters = "png,jpg,jpeg,bmp,tga,psd,gif";

struct DebugOverlayGlobals {
  Shader textureViewShader = Shader{"shaders/default:textureView"};
  gl::Sampler textureViewSampler;
};

static DebugOverlayGlobals &getDebugGlobals() {
  static DebugOverlayGlobals g;
  static bool initialized = false;
  if (!initialized) {
    g.textureViewSampler.setTextureMinFilter(GL_NEAREST);
    g.textureViewSampler.setTextureMagFilter(GL_NEAREST);
    g.textureViewSampler.setWrapModeU(GL_CLAMP_TO_EDGE);
    g.textureViewSampler.setWrapModeV(GL_CLAMP_TO_EDGE);
    g.textureViewSampler.setWrapModeW(GL_CLAMP_TO_EDGE);
    initialized = true;
  }
  return g;
}

static const char *getInternalFormatName(GLenum internalFormat) {
  switch (internalFormat) {
  case GL_R8:
    return "GL_R8";
  case GL_R8_SNORM:
    return "GL_R8_SNORM";
  case GL_R16:
    return "GL_R16";
  case GL_R16_SNORM:
    return "GL_R16_SNORM";
  case GL_RG8:
    return "GL_RG8";
  case GL_RG8_SNORM:
    return "GL_RG8_SNORM";
  case GL_RG16:
    return "GL_RG16";
  case GL_RG16_SNORM:
    return "GL_RG16_SNORM";
  case GL_R3_G3_B2:
    return "GL_R3_G3_B2";
  case GL_RGB4:
    return "GL_RGB4";
  case GL_RGB5:
    return "GL_RGB5";
  case GL_RGB8:
    return "GL_RGB8";
  case GL_RGB8_SNORM:
    return "GL_RGB8_SNORM";
  case GL_RGB10:
    return "GL_RGB10";
  case GL_RGB12:
    return "GL_RGB12";
  case GL_RGB16_SNORM:
    return "GL_RGB16_SNORM";
  case GL_RGBA2:
    return "GL_RGBA2";
  case GL_RGBA4:
    return "GL_RGBA4";
  case GL_RGB5_A1:
    return "GL_RGB5_A1";
  case GL_RGBA8:
    return "GL_RGBA8";
  case GL_RGBA8_SNORM:
    return "GL_RGBA8_SNORM";
  case GL_RGB10_A2:
    return "GL_RGB10_A2";
  case GL_RGB10_A2UI:
    return "GL_RGB10_A2UI";
  case GL_RGBA12:
    return "GL_RGBA12";
  case GL_RGBA16:
    return "GL_RGBA16";
  case GL_SRGB8:
    return "GL_SRGB8";
  case GL_SRGB8_ALPHA8:
    return "GL_SRGB8_ALPHA8";
  case GL_R16F:
    return "GL_R16F";
  case GL_RG16F:
    return "GL_RG16F";
  case GL_RGB16F:
    return "GL_RGB16F";
  case GL_RGBA16F:
    return "GL_RGBA16F";
  case GL_R32F:
    return "GL_R32F";
  case GL_RG32F:
    return "GL_RG32F";
  case GL_RGB32F:
    return "GL_RGB32F";
  case GL_RGBA32F:
    return "GL_RGBA32F";
  case GL_R11F_G11F_B10F:
    return "GL_R11F_G11F_B10F";
  case GL_RGB9_E5:
    return "GL_RGB9_E5";
  case GL_R8I:
    return "GL_R8I";
  case GL_R8UI:
    return "GL_R8UI";
  case GL_R16I:
    return "GL_R16I";
  case GL_R16UI:
    return "GL_R16UI";
  case GL_R32I:
    return "GL_R32I";
  case GL_R32UI:
    return "GL_R32UI";
  case GL_RG8I:
    return "GL_RG8I";
  case GL_RG8UI:
    return "GL_RG8UI";
  case GL_RG16I:
    return "GL_RG16I";
  case GL_RG16UI:
    return "GL_RG16UI";
  case GL_RG32I:
    return "GL_RG32I";
  case GL_RG32UI:
    return "GL_RG32UI";
  case GL_RGB8I:
    return "GL_RGB8I";
  case GL_RGB8UI:
    return "GL_RGB8UI";
  case GL_RGB16I:
    return "GL_RGB16I";
  case GL_RGB16UI:
    return "GL_RGB16UI";
  case GL_RGB32I:
    return "GL_RGB32I";
  case GL_RGB32UI:
    return "GL_RGB32UI";
  case GL_RGBA8I:
    return "GL_RGBA8I";
  case GL_RGBA8UI:
    return "GL_RGBA8UI";
  case GL_RGBA16I:
    return "GL_RGBA16I";
  case GL_RGBA16UI:
    return "GL_RGBA16UI";
  case GL_RGBA32I:
    return "GL_RGBA32I";
  case GL_RGBA32UI:
    return "GL_RGBA32UI";
  case GL_DEPTH_COMPONENT16:
    return "GL_DEPTH_COMPONENT16";
  case GL_DEPTH_COMPONENT24:
    return "GL_DEPTH_COMPONENT24";
  case GL_DEPTH_COMPONENT32:
    return "GL_DEPTH_COMPONENT32";
  case GL_DEPTH_COMPONENT32F:
    return "GL_DEPTH_COMPONENT32F";
  case GL_DEPTH24_STENCIL8:
    return "GL_DEPTH24_STENCIL8";
  case GL_DEPTH32F_STENCIL8:
    return "GL_DEPTH32F_STENCIL8";
  case GL_STENCIL_INDEX8:
    return "GL_STENCIL_INDEX8";
  case GL_COMPRESSED_RED:
    return "GL_COMPRESSED_RED";
  case GL_COMPRESSED_RG:
    return "GL_COMPRESSED_RG";
  case GL_COMPRESSED_RGB:
    return "GL_COMPRESSED_RGB";
  case GL_COMPRESSED_RGBA:
    return "GL_COMPRESSED_RGBA";
  case GL_COMPRESSED_SRGB:
    return "GL_COMPRESSED_SRGB";
  case GL_COMPRESSED_SRGB_ALPHA:
    return "GL_COMPRESSED_SRGB_ALPHA";
  case GL_COMPRESSED_RED_RGTC1:
    return "GL_COMPRESSED_RED_RGTC1";
  case GL_COMPRESSED_SIGNED_RED_RGTC1:
    return "GL_COMPRESSED_SIGNED_RED_RGTC1";
  case GL_COMPRESSED_RG_RGTC2:
    return "GL_COMPRESSED_RG_RGTC2";
  case GL_COMPRESSED_SIGNED_RG_RGTC2:
    return "GL_COMPRESSED_SIGNED_RG_RGTC2";
  case GL_COMPRESSED_RGBA_BPTC_UNORM:
    return "GL_COMPRESSED_RGBA_BPTC_UNORM";
  case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
    return "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
  case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
    return "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
  case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
    return "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
  case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    return "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
  case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
    return "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT";
  case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    return "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";
  case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
    return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
  case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
    return "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT";
  case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
    return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
  case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
    return "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT";
  case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
    return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
  default:
    return "<unknown GL internal format>";
  }
}

struct OpenGLState {
  GLint last_program;
  GLint last_texture;
  GLint last_active_texture;
  GLint last_array_buffer;
  GLint last_element_array_buffer;
  GLint last_vertex_array;
  GLint last_blend_src;
  GLint last_blend_dst;
  GLint last_blend_equation_rgb;
  GLint last_blend_equation_alpha;
  GLint last_viewport[4];
  GLboolean last_enable_blend;
  GLboolean last_enable_cull_face;
  GLboolean last_enable_depth_test;
  GLboolean last_enable_scissor_test;
};

static void saveOpenGLState(OpenGLState &outSavedState) {
  glGetIntegerv(GL_CURRENT_PROGRAM, &outSavedState.last_program);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &outSavedState.last_texture);
  glGetIntegerv(GL_ACTIVE_TEXTURE, &outSavedState.last_active_texture);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &outSavedState.last_array_buffer);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,
                &outSavedState.last_element_array_buffer);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &outSavedState.last_vertex_array);
  glGetIntegerv(GL_BLEND_SRC, &outSavedState.last_blend_src);
  glGetIntegerv(GL_BLEND_DST, &outSavedState.last_blend_dst);
  glGetIntegerv(GL_BLEND_EQUATION_RGB, &outSavedState.last_blend_equation_rgb);
  glGetIntegerv(GL_BLEND_EQUATION_ALPHA,
                &outSavedState.last_blend_equation_alpha);
  glGetIntegerv(GL_VIEWPORT, outSavedState.last_viewport);
  outSavedState.last_enable_blend = glIsEnabled(GL_BLEND);
  outSavedState.last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  outSavedState.last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  outSavedState.last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
}

static void restoreOpenGLState(const OpenGLState &savedState) {
  glUseProgram(savedState.last_program);
  glActiveTexture(savedState.last_active_texture);
  glBindTexture(GL_TEXTURE_2D, savedState.last_texture);
  glBindVertexArray(savedState.last_vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, savedState.last_array_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, savedState.last_element_array_buffer);
  glBlendEquationSeparate(savedState.last_blend_equation_rgb,
                          savedState.last_blend_equation_alpha);
  glBlendFunc(savedState.last_blend_src, savedState.last_blend_dst);
  if (savedState.last_enable_blend)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);
  if (savedState.last_enable_cull_face)
    glEnable(GL_CULL_FACE);
  else
    glDisable(GL_CULL_FACE);
  if (savedState.last_enable_depth_test)
    glEnable(GL_DEPTH_TEST);
  else
    glDisable(GL_DEPTH_TEST);
  if (savedState.last_enable_scissor_test)
    glEnable(GL_SCISSOR_TEST);
  else
    glDisable(GL_SCISSOR_TEST);
  glViewport(savedState.last_viewport[0], savedState.last_viewport[1],
             (GLsizei)savedState.last_viewport[2],
             (GLsizei)savedState.last_viewport[3]);
}

static void beginFixedTooltip(const char *id) {
  ImGui::PushStyleColor(ImGuiCol_WindowBg,
                        ImGui::GetStyle().Colors[ImGuiCol_PopupBg]);
  ImGui::Begin(id, nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoResize);
}

static void endFixedTooltip() {
  ImGui::End();
  ImGui::PopStyleColor();
}

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

static void texturePreview(GLuint textureObj, int w, int h, float lod,
                           int xoffset, int yoffset, float zoomLevel, bool mirror = false,
                           float range_min = 0.0f, float range_max = 1.0f) {
  ImGui::Dummy(ImVec2{(float)w, (float)h});
  auto pos = ImGui::GetItemRectMin();
  auto size = ImGui::GetItemRectSize();
  customRendering([=](const ImDrawList *parentList, const ImDrawCmd *cmd) {
    auto &fb = gl::getDefaultFramebuffer();
    auto fb_width = fb.width();
    auto fb_height = fb.height();
	float uv_l = (float)xoffset / (float)w;
	float uv_t = 1.0f / zoomLevel;
	float uv_r = 1.0f / zoomLevel;
	float uv_b  = (float)yoffset / (float)h;
	if (mirror) std::swap(uv_t, uv_b);
    ag::gl::drawRect(
        gl::getDefaultFramebuffer(), pos.x, pos.y, pos.x + size.x,
        pos.y + size.y, uv_l, uv_t,
		uv_r, uv_b,
        getDebugGlobals().textureViewShader,
        gl::bind::scissor(0, (int)cmd->ClipRect.x,
                          (int)(fb_height - cmd->ClipRect.w),
                          (int)(cmd->ClipRect.z - cmd->ClipRect.x),
                          (int)(cmd->ClipRect.w - cmd->ClipRect.y)),
        gl::bind::texture(0, textureObj,
                          getDebugGlobals().textureViewSampler.object()),
        gl::bind::uniform_float("uLod", lod),
        gl::bind::uniform_vec2("uRange", vec2(range_min, range_max)),
        gl::bind::uniform_vec4("uBorder", vec4(0.1f, 0.1f, 0.1f, 1.0f)));
  });
}

static void GLTextureViewWindow(GLuint textureObj, int w, int h,
                                GLenum internalFormat, bool &opened) {
  auto internalFormatName = getInternalFormatName(internalFormat);
  auto windowName = fmt::format("Texture object {} ({}x{} {})", textureObj, w,
                                h, internalFormatName);

  if (ImGui::Begin(windowName.c_str(), &opened, ImGuiWindowFlags_MenuBar)) {
    // ImGui::Image(reinterpret_cast<ImTextureID>(textureObj), ImVec2{ (float)w,
    // (float)h });
    float range_min =
        ImGui::GetStateStorage()->GetFloat(ImGui::GetID("range_min"), 0.0f);
    float range_max =
        ImGui::GetStateStorage()->GetFloat(ImGui::GetID("range_max"), 1.0f);
    float zoomLevel =
        ImGui::GetStateStorage()->GetFloat(ImGui::GetID("zoom_level"), 1.0f);
	bool mirror =
		ImGui::GetStateStorage()->GetBool(ImGui::GetID("mirror"), false);
    int xoff = ImGui::GetStateStorage()->GetInt(ImGui::GetID("xoff"), 0);
    int yoff = ImGui::GetStateStorage()->GetInt(ImGui::GetID("yoff"), 0);
    texturePreview(textureObj, w, h, 1.0f, xoff, yoff, zoomLevel, mirror, range_min,
                   range_max);
    vec4 pixel;
    int mx, my;
    bool hovered = false;
    if (ImGui::IsItemHovered()) {
      hovered = true;
      auto pos = ImGui::GetMousePos();
      auto ref = ImGui::GetItemRectMin();
      mx = glm::clamp((int)(pos.x - ref.x), 0, w - 1);
      my = glm::clamp((int)(pos.y - ref.y), 0, h - 1);
      // TODO handle integer textures
      glGetTextureSubImage(textureObj, 0, mx, my, 0, 1, 1, 1, GL_RGBA, GL_FLOAT,
                           4 * 4, &pixel);
    }
    if (ImGui::BeginMenuBar()) {
      if (ImGui::Button(ICON_FA_SEARCH_MINUS)) {
        zoomLevel /= 2;
      }
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_SEARCH_PLUS)) {
        zoomLevel *= 2;
      }
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_ARROWS_ALT)) {
        zoomLevel = 1.0f;
      }
	  ImGui::SameLine();
	  ImGui::Checkbox("Mirror", &mirror);
      ImGui::SameLine();
      ImGui::PushItemWidth(130.0f);
      float zoomLevelPercent = zoomLevel * 100.0f;
      ImGui::SliderFloat("Zoom", &zoomLevelPercent, 1.0f, 400.0f, "%g %%");
      zoomLevel = zoomLevelPercent / 100.0f;
      ImGui::SameLine();
      ImGui::SliderFloat("Min", &range_min, 0.0f, 1.0f);
      ImGui::SameLine();
      ImGui::SliderFloat("Max", &range_max, 0.0f, 1.0f);
      ImGui::PopItemWidth();
      ImGui::SameLine();
      if (hovered) {
        ImGui::PushItemWidth(-1.0f);
        ImGui::Text("%05i,%05i => [%.03f, %.03f, %.03f, %.03f]", mx, my,
                    pixel.r, pixel.g, pixel.b, pixel.a);
        ImGui::PopItemWidth();
      }
      ImGui::EndMenuBar();
    }
	ImGui::GetStateStorage()->SetBool(ImGui::GetID("mirror"), mirror);
    ImGui::GetStateStorage()->SetFloat(ImGui::GetID("range_min"), range_min);
    ImGui::GetStateStorage()->SetFloat(ImGui::GetID("range_max"), range_max);
    ImGui::GetStateStorage()->SetFloat(ImGui::GetID("zoom_level"), zoomLevel);
    ImGui::GetStateStorage()->SetInt(ImGui::GetID("xoff"), xoff);
    ImGui::GetStateStorage()->SetInt(ImGui::GetID("yoff"), yoff);
  }
  ImGui::End();
}

static void GLTextureGUI(GLuint textureObj) {
  int w, h;
  GLint internalFormat;
  glGetTextureLevelParameteriv(textureObj, 0, GL_TEXTURE_WIDTH, &w);
  glGetTextureLevelParameteriv(textureObj, 0, GL_TEXTURE_HEIGHT, &h);
  glGetTextureLevelParameteriv(textureObj, 0, GL_TEXTURE_INTERNAL_FORMAT,
                               &internalFormat);
  auto imguiState = ImGui::GetStateStorage();
  auto id = ImGui::GetID("texview");
  bool opened = imguiState->GetBool(id);
  if (ImGui::Button("Toggle view texture")) {
    opened = true;
  }
  if (opened) {
    GLTextureViewWindow(textureObj, w, h, internalFormat, opened);
  }
  imguiState->SetBool(id, opened);

  if (ImGui::Button("Load texture...")) {
    auto file =
        openFileDialog(imageFileFilters, getProjectRootDirectory().c_str());
    if (file) {
      try {
        auto img = loadImage(file->c_str());
        if (img.desc.width == w && img.desc.height == h) {
          // dimensions match, continue
          // as usual, let OpenGL do the pixel conversion
          glTextureSubImage2D(textureObj, 0, 0, 0, w, h, GL_RGBA,
                              GL_UNSIGNED_INT_8_8_8_8, img.data.get());
        } else {
          errorMessage("Error loading texture: mismatched dimensions ({}x{})",
                       img.desc.width, img.desc.height);
        }
      } catch (std::exception &e) {
        errorMessage("Error loading image file: {}", e.what());
      }
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Save texture...")) {
    auto file =
        saveFileDialog(imageFileFilters, getProjectRootDirectory().c_str());
    if (file) {
      try {
        // allocate space for image
        auto bufsize = w * h * 4;
        std::vector<char> pixels(bufsize);
        glGetTextureSubImage(textureObj, 0, 0, 0, 0, w, h, 1, GL_RGBA,
                             GL_UNSIGNED_INT_8_8_8_8, bufsize, pixels.data());
        // Note: assume that the pixels are SRGB, which is usually totally wrong
        // Should do a conversion before
        saveImageByPath(file->c_str(), pixels.data(), w, h,
                        ImageFormat::R8G8B8A8_UNORM);
      } catch (std::exception &e) {
        errorMessage("Error saving image file: {}", e.what());
      }
    }
  }
  // auto& defaultFbo = gl::getDefaultFramebuffer();
  // int screenW = defaultFbo.width();
  // int screenH = defaultFbo.height();
  // getRenderUtils().drawSprite(gl::getDefaultFramebuffer(), 0.0f, 0.0f,
  // (float)w, (float)h, textureObj);
}

static void GLObjectListGUI() {
  ImGui::Begin("GL objects");
  static int selected = -1;
  auto objCount = gl::getGLObjectCount();
  if (selected >= objCount)
    selected = objCount - 1;

  //////////////////////////////////////////
  ImGui::BeginChild("Object list",
                    ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f,
                           ImGui::GetContentRegionAvail().y),
                    true, ImGuiWindowFlags_HorizontalScrollbar);
  ImGui::Columns(3);
  ImGui::Text("Object ID");
  ImGui::NextColumn();
  ImGui::Text("Type");
  ImGui::NextColumn();
  ImGui::Text("Frame created");
  ImGui::NextColumn();
  ImGui::Separator();
  for (int i = 0; i < objCount; ++i) {
    ImGui::PushID(i);
    auto obj = gl::getGLObjectData(i);
    auto objStr = fmt::format("({}):{}", obj->type, obj->obj);
    bool isSelected = selected == i;
    if (ImGui::Selectable(objStr.c_str(), &isSelected,
                          ImGuiSelectableFlags_SpanAllColumns)) {
      selected = i;
    }
    ImGui::NextColumn();
    ImGui::Text("%s", gl::getGLObjectTypeName(obj->type));
    ImGui::NextColumn();
    ImGui::Text("%" PRIi64, obj->creationFrame);
    ImGui::NextColumn();
    ImGui::PopID();
  }
  ImGui::EndChild();

  //////////////////////////////////////////
  if (selected != -1) {
    ImGui::SameLine();
    ImGui::BeginChild("Object GUI", ImVec2(0, 300), false);
    auto obj = gl::getGLObjectData(selected);
    if (obj->type == GL_TEXTURE)
      GLTextureGUI(obj->obj);
    ImGui::EndChild();
  }

  ImGui::End();
}

using NameValuePair = std::pair<const char *, GLenum>;

static void ComboGLenum(const char *label, GLenum *outValue,
                        span<const NameValuePair> values) {
  auto enumToIndex = [&](GLenum e) {
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
  *outValue = values[curIdx].second;
}

static void pipelineStateGUI(PipelineState *ps) {
  //////////////////////////////////////////////////////
  // Depth-stencil
  if (ImGui::CollapsingHeader("Depth-stencil state")) {
    ImGui::Checkbox("Depth test enabled",
                    &ps->drawStates.depthStencilState.depthTestEnable);
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("glEnable(GL_DEPTH_TEST)");
    }
    ImGui::Checkbox("Depth write enabled",
                    &ps->drawStates.depthStencilState.depthWriteEnable);
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Write to the depth buffer");
    }

    static const NameValuePair depthTestFuncNames[] = {
        {"GL_NEVER", GL_NEVER},     {"GL_LESS", GL_LESS},
        {"GL_EQUAL", GL_EQUAL},     {"GL_LEQUAL", GL_LEQUAL},
        {"GL_GREATER", GL_GREATER}, {"GL_NOTEQUAL", GL_NOTEQUAL},
        {"GL_GEQUAL", GL_GEQUAL},   {"GL_ALWAYS", GL_ALWAYS}};
    ComboGLenum("Depth test func",
                &ps->drawStates.depthStencilState.depthTestFunc,
                depthTestFuncNames);
  }

  //////////////////////////////////////////////////////
  // Rasterizer
  if (ImGui::CollapsingHeader("Rasterizer state")) {
    static const NameValuePair fillModeNames[] = {{"GL_FILL", GL_FILL},
                                                  {"GL_LINE", GL_LINE}};
    ComboGLenum("Fill mode", &ps->drawStates.rasterizerState.fillMode,
                fillModeNames);
  }

  //////////////////////////////////////////////////////
  // Viewports
  if (ImGui::CollapsingHeader("Viewports")) {
    ImGui::Columns(5);
    ImGui::Text("Viewport index");
    ImGui::NextColumn();
    ImGui::Text("X");
    ImGui::NextColumn();
    ImGui::Text("Y");
    ImGui::NextColumn();
    ImGui::Text("W");
    ImGui::NextColumn();
    ImGui::Text("H");
    ImGui::NextColumn();
    ImGui::Separator();

    for (int i = 0; i < 8; ++i) {
      ImGui::PushID(i);
      ImGui::Text("%i", i);
      ImGui::NextColumn();
      ImGui::InputFloat("##X", &ps->drawStates.viewports[i].x);
      ImGui::NextColumn();
      ImGui::InputFloat("##Y", &ps->drawStates.viewports[i].y);
      ImGui::NextColumn();
      ImGui::InputFloat("##Z", &ps->drawStates.viewports[i].w);
      ImGui::NextColumn();
      ImGui::InputFloat("##W", &ps->drawStates.viewports[i].h);
      ImGui::NextColumn();
      ImGui::Separator();
      ImGui::PopID();
    }
    ImGui::Columns(1);
  }

  //////////////////////////////////////////////////////
  // Shader sources
  if (ImGui::CollapsingHeader("Shader sources")) {
    if (ImGui::Button("Recompile")) {
      ps->shouldRecompile = true;
    }
    if (ImGui::Button("VS"))
      ImGui::OpenPopup("Vertex Shader");
    ImGui::SameLine();
    if (ImGui::Button("FS"))
      ImGui::OpenPopup("Fragment Shader");
    ImGui::SameLine();
    if (ImGui::Button("GS"))
      ImGui::OpenPopup("Geometry Shader");
    ImGui::SameLine();
    if (ImGui::Button("TCS"))
      ImGui::OpenPopup("Tess Control Shader");
    ImGui::SameLine();
    if (ImGui::Button("TES"))
      ImGui::OpenPopup("Tess Eval Shader");
    if (ImGui::Button("CS"))
      ImGui::OpenPopup("Compute Shader");
    ImVec2 shaderEditWinSize = ImVec2{200.0f, 400.0f};
    size_t bufSize = 100000;

    auto shaderEditPopup = [&](const char *label, std::string &source) {
      if (ImGui::BeginResizablePopup(label, shaderEditWinSize)) {
        ImGui::PushItemWidth(-1);
        gui::inputTextMultilineString("", source, bufSize);
        ImGui::PopItemWidth();
        ImGui::EndPopup();
      }
    };

    shaderEditPopup("Vertex Shader", ps->vertexShaderSource);
    shaderEditPopup("Fragment Shader", ps->fragmentShaderSource);
    shaderEditPopup("Geometry Shader", ps->geometryShaderSource);
    shaderEditPopup("Tess Control Shader", ps->tessControlShaderSource);
    shaderEditPopup("Tess Eval Shader", ps->tessEvalShaderSource);
    shaderEditPopup("Compute Shader", ps->computeShaderSource);
  }
}

static void pipelineStatesGUI() {
  ImGui::Begin("Shaders");
  auto &pipelineStateCache = getPipelineStateCache();
  if (ImGui::Button("Reload all (Ctrl+F5)")) {
	  pipelineStateCache.reloadAll();
  }
  int numPS = pipelineStateCache.getCachedPipelineStateCount();
  for (int i = 0; i < numPS; ++i) {
    ImGui::PushID(i);
    auto ps = pipelineStateCache.getCachedPipelineState(i);
    if (ImGui::CollapsingHeader(
            fmt::format("#{} | {}", i, ps->origShaderID.c_str()).c_str())) {
      ImGui::Indent(20.0f);
      pipelineStateGUI(ps);
      ImGui::Unindent(20.0f);
    }
    ImGui::PopID();
  }
  ImGui::End();
}

void drawDebugOverlay(double dt) {
  beginFixedTooltip("frame");
  ImGui::TextDisabled("Frame time: %.06f ms (%.02f FPS)", dt * 1000.0f,
                      1.0f / dt);
  endFixedTooltip();
  pipelineStatesGUI();
  GLObjectListGUI();
  showCVarGui();
}
}
