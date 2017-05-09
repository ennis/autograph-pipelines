#include <autograph/Engine/CVar.h>
#include <autograph/Engine/DebugOverlay.h>
#include <autograph/Engine/ImGuiUtils.h>
#include <autograph/Engine/ImageUtils.h>
#include <autograph/Engine/RenderUtils.h>
#include <autograph/Engine/Shader.h>
#include <autograph/Engine/imgui.h>
#include <autograph/Gfx/GfxContext.h>
#include <autograph/Gfx/Draw.h>
#include <autograph/Gfx/GLHandle.h>
#include <autograph/Gfx/Sampler.h>
#include <autograph/Core/Support/Debug.h>
#include <autograph/Core/Support/FileDialog.h>
#include <autograph/Core/Support/ProjectRoot.h>
#include <cinttypes>

namespace ag {

static const char *imageFileFilters = "png,jpg,jpeg,bmp,tga,psd,gif";

struct DebugOverlayGlobals {
  GPUPipeline textureViewShader = GPUPipeline{GPUPipelineType::Graphics, "shaders/default.lua:textureView"};
  Sampler textureViewSampler;
};

static DebugOverlayGlobals &getDebugGlobals() {
  static DebugOverlayGlobals g;
  static bool initialized = false;
  if (!initialized) {
    g.textureViewSampler.setTextureMinFilter(gl::NEAREST);
    g.textureViewSampler.setTextureMagFilter(gl::NEAREST);
    g.textureViewSampler.setWrapModeU(gl::CLAMP_TO_EDGE);
    g.textureViewSampler.setWrapModeV(gl::CLAMP_TO_EDGE);
    g.textureViewSampler.setWrapModeW(gl::CLAMP_TO_EDGE);
    initialized = true;
  }
  return g;
}

static const char *getInternalFormatName(gl::GLenum internalFormat) {
  switch (internalFormat) {
  case gl::R8:
    return "R8";
  case gl::R8_SNORM:
    return "R8_SNORM";
  case gl::R16:
    return "R16";
  case gl::R16_SNORM:
    return "R16_SNORM";
  case gl::RG8:
    return "RG8";
  case gl::RG8_SNORM:
    return "RG8_SNORM";
  case gl::RG16:
    return "RG16";
  case gl::RG16_SNORM:
    return "RG16_SNORM";
  case gl::R3_G3_B2:
    return "R3_G3_B2";
  case gl::RGB4:
    return "RGB4";
  case gl::RGB5:
    return "RGB5";
  case gl::RGB8:
    return "RGB8";
  case gl::RGB8_SNORM:
    return "RGB8_SNORM";
  case gl::RGB10:
    return "RGB10";
  case gl::RGB12:
    return "RGB12";
  case gl::RGB16_SNORM:
    return "RGB16_SNORM";
  case gl::RGBA2:
    return "RGBA2";
  case gl::RGBA4:
    return "RGBA4";
  case gl::RGB5_A1:
    return "RGB5_A1";
  case gl::RGBA8:
    return "RGBA8";
  case gl::RGBA8_SNORM:
    return "RGBA8_SNORM";
  case gl::RGB10_A2:
    return "RGB10_A2";
  case gl::RGB10_A2UI:
    return "RGB10_A2UI";
  case gl::RGBA12:
    return "RGBA12";
  case gl::RGBA16:
    return "RGBA16";
  case gl::SRGB8:
    return "SRGB8";
  case gl::SRGB8_ALPHA8:
    return "SRGB8_ALPHA8";
  case gl::R16F:
    return "R16F";
  case gl::RG16F:
    return "RG16F";
  case gl::RGB16F:
    return "RGB16F";
  case gl::RGBA16F:
    return "RGBA16F";
  case gl::R32F:
    return "R32F";
  case gl::RG32F:
    return "RG32F";
  case gl::RGB32F:
    return "RGB32F";
  case gl::RGBA32F:
    return "RGBA32F";
  case gl::R11F_G11F_B10F:
    return "R11F_G11F_B10F";
  case gl::RGB9_E5:
    return "RGB9_E5";
  case gl::R8I:
    return "R8I";
  case gl::R8UI:
    return "R8UI";
  case gl::R16I:
    return "R16I";
  case gl::R16UI:
    return "R16UI";
  case gl::R32I:
    return "R32I";
  case gl::R32UI:
    return "R32UI";
  case gl::RG8I:
    return "RG8I";
  case gl::RG8UI:
    return "RG8UI";
  case gl::RG16I:
    return "RG16I";
  case gl::RG16UI:
    return "RG16UI";
  case gl::RG32I:
    return "RG32I";
  case gl::RG32UI:
    return "RG32UI";
  case gl::RGB8I:
    return "RGB8I";
  case gl::RGB8UI:
    return "RGB8UI";
  case gl::RGB16I:
    return "RGB16I";
  case gl::RGB16UI:
    return "RGB16UI";
  case gl::RGB32I:
    return "RGB32I";
  case gl::RGB32UI:
    return "RGB32UI";
  case gl::RGBA8I:
    return "RGBA8I";
  case gl::RGBA8UI:
    return "RGBA8UI";
  case gl::RGBA16I:
    return "RGBA16I";
  case gl::RGBA16UI:
    return "RGBA16UI";
  case gl::RGBA32I:
    return "RGBA32I";
  case gl::RGBA32UI:
    return "RGBA32UI";
  case gl::DEPTH_COMPONENT16:
    return "DEPTH_COMPONENT16";
  case gl::DEPTH_COMPONENT24:
    return "DEPTH_COMPONENT24";
  case gl::DEPTH_COMPONENT32:
    return "DEPTH_COMPONENT32";
  case gl::DEPTH_COMPONENT32F:
    return "DEPTH_COMPONENT32F";
  case gl::DEPTH24_STENCIL8:
    return "DEPTH24_STENCIL8";
  case gl::DEPTH32F_STENCIL8:
    return "DEPTH32F_STENCIL8";
  case gl::STENCIL_INDEX8:
    return "STENCIL_INDEX8";
  case gl::COMPRESSED_RED:
    return "COMPRESSED_RED";
  case gl::COMPRESSED_RG:
    return "COMPRESSED_RG";
  case gl::COMPRESSED_RGB:
    return "COMPRESSED_RGB";
  case gl::COMPRESSED_RGBA:
    return "COMPRESSED_RGBA";
  case gl::COMPRESSED_SRGB:
    return "COMPRESSED_SRGB";
  case gl::COMPRESSED_SRGB_ALPHA:
    return "COMPRESSED_SRGB_ALPHA";
  case gl::COMPRESSED_RED_RGTC1:
    return "COMPRESSED_RED_RGTC1";
  case gl::COMPRESSED_SIGNED_RED_RGTC1:
    return "COMPRESSED_SIGNED_RED_RGTC1";
  case gl::COMPRESSED_RG_RGTC2:
    return "COMPRESSED_RG_RGTC2";
  case gl::COMPRESSED_SIGNED_RG_RGTC2:
    return "COMPRESSED_SIGNED_RG_RGTC2";
  case gl::COMPRESSED_RGBA_BPTC_UNORM:
    return "COMPRESSED_RGBA_BPTC_UNORM";
  case gl::COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
    return "COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
  case gl::COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
    return "COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
  case gl::COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
    return "COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
  case gl::COMPRESSED_RGB_S3TC_DXT1_EXT:
    return "COMPRESSED_RGB_S3TC_DXT1_EXT";
  case gl::COMPRESSED_SRGB_S3TC_DXT1_EXT:
    return "COMPRESSED_SRGB_S3TC_DXT1_EXT";
  case gl::COMPRESSED_RGBA_S3TC_DXT1_EXT:
    return "COMPRESSED_RGBA_S3TC_DXT1_EXT";
  case gl::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
    return "COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
  case gl::COMPRESSED_RGBA_S3TC_DXT3_EXT:
    return "COMPRESSED_RGBA_S3TC_DXT3_EXT";
  case gl::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
    return "COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
  case gl::COMPRESSED_RGBA_S3TC_DXT5_EXT:
    return "COMPRESSED_RGBA_S3TC_DXT5_EXT";
  case gl::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
    return "COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
  default:
    return "<unknown GL internal format>";
  }
}

static ImageFormat GLInternalFormatToImageFormat(gl::GLenum glfmt) {
  switch (glfmt) {
  case gl::SRGB8:
    return ImageFormat::R8G8B8_SRGB;
  case gl::SRGB8_ALPHA8:
    return ImageFormat::R8G8B8A8_SRGB;
  case gl::RGBA16F:
    return ImageFormat::R16G16B16A16_SFLOAT;
  case gl::RG16F:
    return ImageFormat::R16G16_SFLOAT;
  default:
    return ImageFormat::R8G8B8A8_SRGB;
  }
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


static void texturePreview(gl::GLuint textureObj, int w, int h, float lod,
                           int xoffset, int yoffset, float zoomLevel,
                           bool mirror = false, float range_min = 0.0f,
                           float range_max = 1.0f) {
  ImGui::Dummy(ImVec2{(float)w, (float)h});
  auto pos = ImGui::GetItemRectMin();
  auto size = ImGui::GetItemRectSize();
  gui::customRendering([=](const ImDrawList *parentList, const ImDrawCmd *cmd) {
	  auto& fb = getGfxContext().getDefaultFramebuffer();
    auto fb_width = fb.width();
    auto fb_height = fb.height();
    float uv_l = (float)xoffset / (float)w;
    float uv_t = 1.0f / zoomLevel;
    float uv_r = 1.0f / zoomLevel;
    float uv_b = (float)yoffset / (float)h;
    if (mirror)
      std::swap(uv_t, uv_b);
    ag::drawRect(fb, pos.x, pos.y, pos.x + size.x,
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

static void GLTextureViewWindow(gl::GLuint textureObj, int w, int h,
                                gl::GLenum internalFormat, bool &opened) {
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
    texturePreview(textureObj, w, h, 1.0f, xoff, yoff, zoomLevel, mirror,
                   range_min, range_max);
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
      gl::GetTextureSubImage(textureObj, 0, mx, my, 0, 1, 1, 1, gl::RGBA,
                             gl::FLOAT, 4 * 4, &pixel);
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

static void saveGLTexture(const char *path, gl::GLuint textureObj, int w, int h,
                          gl::GLenum internalFormat) {
  ImageFormat fmt;
  gl::GLenum extFormat;
  gl::GLenum components;
  int size;

  switch (internalFormat) {
  case gl::RGBA16F:
    fmt = ImageFormat::R16G16B16A16_SFLOAT;
    extFormat = gl::HALF_FLOAT;
    components = gl::RGBA;
    size = 8;
    break;
  case gl::RG16F:
    fmt = ImageFormat::R16G16_SFLOAT;
    extFormat = gl::HALF_FLOAT;
    components = gl::RG;
    size = 4;
    break;
  case gl::RGBA8_SNORM:
    fmt = ImageFormat::R8G8B8A8_SNORM;
    extFormat = gl::UNSIGNED_INT_8_8_8_8_REV;
    components = gl::RGBA;
    size = 4;
    break;
  case gl::RGBA8:
    fmt = ImageFormat::R8G8B8A8_UNORM;
    extFormat = gl::UNSIGNED_INT_8_8_8_8_REV;
    components = gl::RGBA;
    size = 4;
    break;
  case gl::SRGB8_ALPHA8:
    fmt = ImageFormat::R8G8B8A8_SRGB;
    extFormat = gl::UNSIGNED_INT_8_8_8_8_REV;
    components = gl::RGBA;
    size = 4;
    break;
  default:
    errorMessage("saveGLTexture: texture format not supported ({})",
                 getInternalFormatName(internalFormat));
    return;
  }

  // allocate space for image
  auto bufsize = w * h * size;
  std::vector<char> pixels(bufsize);
  gl::GetTextureSubImage(textureObj, 0, 0, 0, 0, w, h, 1, components, extFormat,
                         bufsize, pixels.data());
  saveImageByPath(path, pixels.data(), w, h, fmt, fmt);
}

static void GLTextureGUI(gl::GLuint textureObj) {
  int w, h;
  gl::GLint internalFormat;
  gl::GetTextureLevelParameteriv(textureObj, 0, gl::TEXTURE_WIDTH, &w);
  gl::GetTextureLevelParameteriv(textureObj, 0, gl::TEXTURE_HEIGHT, &h);
  gl::GetTextureLevelParameteriv(textureObj, 0, gl::TEXTURE_INTERNAL_FORMAT,
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
          gl::TextureSubImage2D(textureObj, 0, 0, 0, w, h, gl::RGBA,
                                gl::UNSIGNED_INT_8_8_8_8, img.data.get());
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
        saveGLTexture(file->c_str(), textureObj, w, h, internalFormat);
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
  auto objCount = getGLObjectCount();
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
    auto obj = getGLObjectData(i);
    auto objStr = fmt::format("({}):{}", obj->type, obj->obj);
    bool isSelected = selected == i;
    if (ImGui::Selectable(objStr.c_str(), &isSelected,
                          ImGuiSelectableFlags_SpanAllColumns)) {
      selected = i;
    }
    ImGui::NextColumn();
    ImGui::Text("%s", getGLObjectTypeName(obj->type));
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
    auto obj = getGLObjectData(selected);
    if (obj->type == gl::TEXTURE)
      GLTextureGUI(obj->obj);
    ImGui::EndChild();
  }

  ImGui::End();
}

using NameValuePair = std::pair<const char *, gl::GLenum>;

static void ComboGLenum(const char *label, gl::GLenum *outValue,
                        span<const NameValuePair> values) {
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
  *outValue = values[curIdx].second;
}

/*static void pipelineStateGUI(PipelineState *ps) {
  //////////////////////////////////////////////////////
  // Depth-stencil
  if (ImGui::CollapsingHeader("Depth-stencil state")) {
    ImGui::Checkbox("Depth test enabled",
                    &ps->drawStates.depthStencilState.depthTestEnable);
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("gl::Enable(gl::DEPTH_TEST)");
    }
    ImGui::Checkbox("Depth write enabled",
                    &ps->drawStates.depthStencilState.depthWriteEnable);
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Write to the depth buffer");
    }

    static const NameValuePair depthTestFuncNames[] = {
        {"gl::NEVER", gl::NEVER},     {"gl::LESS", gl::LESS},
        {"gl::EQUAL", gl::EQUAL},     {"gl::LEQUAL", gl::LEQUAL},
        {"gl::GREATER", gl::GREATER}, {"gl::NOTEQUAL", gl::NOTEQUAL},
        {"gl::GEQUAL", gl::GEQUAL},   {"gl::ALWAYS", gl::ALWAYS}};
    ComboGLenum("Depth test func",
                &ps->drawStates.depthStencilState.depthTestFunc,
                depthTestFuncNames);
  }

  //////////////////////////////////////////////////////
  // Rasterizer
  if (ImGui::CollapsingHeader("Rasterizer state")) {
    static const NameValuePair fillModeNames[] = {{"gl::FILL", gl::FILL},
                                                  {"gl::LINE", gl::LINE}};
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
}*/

void drawDebugOverlay(double dt) {
  beginFixedTooltip("frame");
  ImGui::TextDisabled("Frame time: %.06f ms (%.02f FPS)", dt * 1000.0f,
                      1.0f / dt);
  endFixedTooltip();
  //pipelineStatesGUI();
  GLObjectListGUI();
  showCVarGui();
}
} // namespace ag
