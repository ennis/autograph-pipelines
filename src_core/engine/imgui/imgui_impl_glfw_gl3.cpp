// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture
// identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See
// main.cpp for an example of using this. If you use this binding you'll need to
// call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(),
// ImGui::Render() and ImGui_ImplXXXX_Shutdown(). If you are new to ImGui, see
// examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <autograph/engine/imgui.h>
#include "imgui_impl_glfw_gl3.h"
// GL3W/GLFW
#include <GLFW/glfw3.h>
#include <autograph/gl/gl_core_4_5.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

// Data
static GLFWwindow *g_Window = NULL;
static double g_Time = 0.0f;
static bool g_MousePressed[3] = {false, false, false};
static float g_MouseWheel = 0.0f;
static GLuint g_FontTexture = 0;
static int g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int g_AttribLocationPosition = 0, g_AttribLocationUV = 0,
           g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

using namespace ag;

// This is the main rendering function that you have to implement and provide to
// ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure) If text
// or lines are blurry when integrating ImGui in your engine: - in your Render
// function, try translating your projection matrix by (0.5f,0.5f) or
// (0.375f,0.375f)
void ImGui_ImplGlfwGL3_RenderDrawLists(ImDrawData *draw_data) {
  // Avoid rendering when minimized, scale coordinates for retina displays
  // (screen coordinates != framebuffer coordinates)
  ImGuiIO &io = ImGui::GetIO();
  int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  if (fb_width == 0 || fb_height == 0)
    return;
  draw_data->ScaleClipRects(io.DisplayFramebufferScale);

  // Backup GL state
  GLint last_program;
  gl::GetIntegerv(gl::CURRENT_PROGRAM, &last_program);
  GLint last_texture;
  gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);
  GLint last_active_texture;
  gl::GetIntegerv(gl::ACTIVE_TEXTURE, &last_active_texture);
  GLint last_array_buffer;
  gl::GetIntegerv(gl::ARRAY_BUFFER_BINDING, &last_array_buffer);
  GLint last_element_array_buffer;
  gl::GetIntegerv(gl::ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
  GLint last_vertex_array;
  gl::GetIntegerv(gl::VERTEX_ARRAY_BINDING, &last_vertex_array);
  GLint last_blend_src;
  gl::GetIntegerv(gl::BLEND_SRC, &last_blend_src);
  GLint last_blend_dst;
  gl::GetIntegerv(gl::BLEND_DST, &last_blend_dst);
  GLint last_blend_equation_rgb;
  gl::GetIntegerv(gl::BLEND_EQUATION_RGB, &last_blend_equation_rgb);
  GLint last_blend_equation_alpha;
  gl::GetIntegerv(gl::BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
  GLint last_viewport[4];
  gl::GetIntegerv(gl::VIEWPORT, last_viewport);
  GLboolean last_enable_blend = gl::IsEnabled(gl::BLEND);
  GLboolean last_enable_cull_face = gl::IsEnabled(gl::CULL_FACE);
  GLboolean last_enable_depth_test = gl::IsEnabled(gl::DEPTH_TEST);
  GLboolean last_enable_scissor_test = gl::IsEnabled(gl::SCISSOR_TEST);
  GLboolean last_enable_framebuffer_srgb = gl::IsEnabled(gl::FRAMEBUFFER_SRGB);

  // Setup render state: alpha-blending enabled, no face culling, no depth
  // testing, scissor enabled
  gl::Enable(gl::BLEND);
  gl::BlendEquation(gl::FUNC_ADD);
  gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);
  gl::Disable(gl::CULL_FACE);
  gl::Disable(gl::DEPTH_TEST);
  gl::Enable(gl::SCISSOR_TEST);
  gl::ActiveTexture(gl::TEXTURE0);
  gl::Disable(gl::FRAMEBUFFER_SRGB);

  // Setup viewport, orthographic projection matrix
  gl::Viewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  const float ortho_projection[4][4] = {
      {2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
      {0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
      {0.0f, 0.0f, -1.0f, 0.0f},
      {-1.0f, 1.0f, 0.0f, 1.0f},
  };
  gl::UseProgram(g_ShaderHandle);
  gl::Uniform1i(g_AttribLocationTex, 0);
  gl::UniformMatrix4fv(g_AttribLocationProjMtx, 1, gl::FALSE_,
                       &ortho_projection[0][0]);
  gl::BindVertexArray(g_VaoHandle);

  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList *cmd_list = draw_data->CmdLists[n];
    const ImDrawIdx *idx_buffer_offset = 0;

    gl::BindBuffer(gl::ARRAY_BUFFER, g_VboHandle);
    gl::BufferData(gl::ARRAY_BUFFER,
                   (gl::GLsizeiptr)cmd_list->VtxBuffer.size() *
                       sizeof(ImDrawVert),
                   (GLvoid *)&cmd_list->VtxBuffer.front(), gl::STREAM_DRAW);

    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER,
                   (gl::GLsizeiptr)cmd_list->IdxBuffer.size() *
                       sizeof(ImDrawIdx),
                   (GLvoid *)&cmd_list->IdxBuffer.front(), gl::STREAM_DRAW);

    for (const ImDrawCmd *pcmd = cmd_list->CmdBuffer.begin();
         pcmd != cmd_list->CmdBuffer.end(); pcmd++) {
      if (pcmd->UserCallback) {
        pcmd->UserCallback(cmd_list, pcmd);
      } else {
        gl::BindTexture(gl::TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
        gl::Scissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w),
                    (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                    (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        gl::DrawElements(gl::TRIANGLES, (GLsizei)pcmd->ElemCount,
                         sizeof(ImDrawIdx) == 2 ? gl::UNSIGNED_SHORT
                                                : gl::UNSIGNED_INT,
                         idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }

  // Restore modified GL state
  gl::UseProgram(last_program);
  gl::ActiveTexture(last_active_texture);
  gl::BindTexture(gl::TEXTURE_2D, last_texture);
  gl::BindVertexArray(last_vertex_array);
  gl::BindBuffer(gl::ARRAY_BUFFER, last_array_buffer);
  gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
  gl::BlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
  gl::BlendFunc(last_blend_src, last_blend_dst);
  if (last_enable_blend)
    gl::Enable(gl::BLEND);
  else
    gl::Disable(gl::BLEND);
  if (last_enable_cull_face)
    gl::Enable(gl::CULL_FACE);
  else
    gl::Disable(gl::CULL_FACE);
  if (last_enable_depth_test)
    gl::Enable(gl::DEPTH_TEST);
  else
    gl::Disable(gl::DEPTH_TEST);
  if (last_enable_scissor_test)
    gl::Enable(gl::SCISSOR_TEST);
  else
    gl::Disable(gl::SCISSOR_TEST);
  if (last_enable_framebuffer_srgb)
    gl::Enable(gl::FRAMEBUFFER_SRGB);
  else
    gl::Disable(gl::FRAMEBUFFER_SRGB);
  gl::Viewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2],
               (GLsizei)last_viewport[3]);
}

static const char *ImGui_ImplGlfwGL3_GetClipboardText(void *userdata) {
  return glfwGetClipboardString(g_Window);
}

static void ImGui_ImplGlfwGL3_SetClipboardText(void *userdata,
                                               const char *text) {
  glfwSetClipboardString(g_Window, text);
}

void ImGui_ImplGlfwGL3_MouseButtonCallback(GLFWwindow *, int button, int action,
                                           int /*mods*/) {
  if (action == GLFW_PRESS && button >= 0 && button < 3)
    g_MousePressed[button] = true;
}

void ImGui_ImplGlfwGL3_ScrollCallback(GLFWwindow *, double /*xoffset*/,
                                      double yoffset) {
  g_MouseWheel +=
      (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
}

void ImGui_ImplGlfwGL3_KeyCallback(GLFWwindow *, int key, int, int action,
                                   int mods) {
  ImGuiIO &io = ImGui::GetIO();
  if (action == GLFW_PRESS)
    io.KeysDown[key] = true;
  if (action == GLFW_RELEASE)
    io.KeysDown[key] = false;

  (void)mods; // Modifiers are not reliable across systems
  io.KeyCtrl =
      io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift =
      io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
  io.KeySuper =
      io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void ImGui_ImplGlfwGL3_CharCallback(GLFWwindow *, unsigned int c) {
  ImGuiIO &io = ImGui::GetIO();
  if (c > 0 && c < 0x10000)
    io.AddInputCharacter((unsigned short)c);
}

bool ImGui_ImplGlfwGL3_CreateFontsTexture() {
  // Build texture atlas
  ImGuiIO &io = ImGui::GetIO();
  unsigned char *pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(
      &pixels, &width, &height); // Load as RGBA 32-bits for OpenGL3 demo
                                 // because it is more likely to be compatible
                                 // with user's existing shader.

  // Upload texture to graphics system
  GLint last_texture;
  gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);
  gl::GenTextures(1, &g_FontTexture);
  gl::BindTexture(gl::TEXTURE_2D, g_FontTexture);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::LINEAR);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::LINEAR);
  gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RGBA, width, height, 0, gl::RGBA,
                 gl::UNSIGNED_BYTE, pixels);

  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

  // Restore state
  gl::BindTexture(gl::TEXTURE_2D, last_texture);

  return true;
}

bool ImGui_ImplGlfwGL3_CreateDeviceObjects() {
  // Backup GL state
  GLint last_texture, last_array_buffer, last_vertex_array;
  gl::GetIntegerv(gl::TEXTURE_BINDING_2D, &last_texture);
  gl::GetIntegerv(gl::ARRAY_BUFFER_BINDING, &last_array_buffer);
  gl::GetIntegerv(gl::VERTEX_ARRAY_BINDING, &last_vertex_array);

  const gl::GLchar *vertex_shader =
      "#version 330\n"
      "uniform mat4 ProjMtx;\n"
      "in vec2 Position;\n"
      "in vec2 UV;\n"
      "in vec4 Color;\n"
      "out vec2 Frag_UV;\n"
      "out vec4 Frag_Color;\n"
      "void main()\n"
      "{\n"
      "	Frag_UV = UV;\n"
      "	Frag_Color = Color;\n"
      "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
      "}\n";

  const gl::GLchar *fragment_shader =
      "#version 330\n"
      "uniform sampler2D Texture;\n"
      "in vec2 Frag_UV;\n"
      "in vec4 Frag_Color;\n"
      "out vec4 Out_Color;\n"
      "void main()\n"
      "{\n"
      "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
      "}\n";

  g_ShaderHandle = gl::CreateProgram();
  g_VertHandle = gl::CreateShader(gl::VERTEX_SHADER);
  g_FragHandle = gl::CreateShader(gl::FRAGMENT_SHADER);
  gl::ShaderSource(g_VertHandle, 1, &vertex_shader, 0);
  gl::ShaderSource(g_FragHandle, 1, &fragment_shader, 0);
  gl::CompileShader(g_VertHandle);
  gl::CompileShader(g_FragHandle);
  gl::AttachShader(g_ShaderHandle, g_VertHandle);
  gl::AttachShader(g_ShaderHandle, g_FragHandle);
  gl::LinkProgram(g_ShaderHandle);

  g_AttribLocationTex = gl::GetUniformLocation(g_ShaderHandle, "Texture");
  g_AttribLocationProjMtx = gl::GetUniformLocation(g_ShaderHandle, "ProjMtx");
  g_AttribLocationPosition = gl::GetAttribLocation(g_ShaderHandle, "Position");
  g_AttribLocationUV = gl::GetAttribLocation(g_ShaderHandle, "UV");
  g_AttribLocationColor = gl::GetAttribLocation(g_ShaderHandle, "Color");

  gl::GenBuffers(1, &g_VboHandle);
  gl::GenBuffers(1, &g_ElementsHandle);

  gl::GenVertexArrays(1, &g_VaoHandle);
  gl::BindVertexArray(g_VaoHandle);
  gl::BindBuffer(gl::ARRAY_BUFFER, g_VboHandle);
  gl::EnableVertexAttribArray(g_AttribLocationPosition);
  gl::EnableVertexAttribArray(g_AttribLocationUV);
  gl::EnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE *)0)->ELEMENT))
  gl::VertexAttribPointer(g_AttribLocationPosition, 2, gl::FLOAT, gl::FALSE_,
                          sizeof(ImDrawVert),
                          (GLvoid *)OFFSETOF(ImDrawVert, pos));
  gl::VertexAttribPointer(g_AttribLocationUV, 2, gl::FLOAT, gl::FALSE_,
                          sizeof(ImDrawVert),
                          (GLvoid *)OFFSETOF(ImDrawVert, uv));
  gl::VertexAttribPointer(g_AttribLocationColor, 4, gl::UNSIGNED_BYTE,
                          gl::TRUE_, sizeof(ImDrawVert),
                          (GLvoid *)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

  ImGui_ImplGlfwGL3_CreateFontsTexture();

  // Restore modified GL state
  gl::BindTexture(gl::TEXTURE_2D, last_texture);
  gl::BindBuffer(gl::ARRAY_BUFFER, last_array_buffer);
  gl::BindVertexArray(last_vertex_array);

  return true;
}

void ImGui_ImplGlfwGL3_InvalidateDeviceObjects() {
  if (g_VaoHandle)
    gl::DeleteVertexArrays(1, &g_VaoHandle);
  if (g_VboHandle)
    gl::DeleteBuffers(1, &g_VboHandle);
  if (g_ElementsHandle)
    gl::DeleteBuffers(1, &g_ElementsHandle);
  g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

  gl::DetachShader(g_ShaderHandle, g_VertHandle);
  gl::DeleteShader(g_VertHandle);
  g_VertHandle = 0;

  gl::DetachShader(g_ShaderHandle, g_FragHandle);
  gl::DeleteShader(g_FragHandle);
  g_FragHandle = 0;

  gl::DeleteProgram(g_ShaderHandle);
  g_ShaderHandle = 0;

  if (g_FontTexture) {
    gl::DeleteTextures(1, &g_FontTexture);
    ImGui::GetIO().Fonts->TexID = 0;
    g_FontTexture = 0;
  }
}

bool ImGui_ImplGlfwGL3_Init(GLFWwindow *window, bool install_callbacks) {
  g_Window = window;

  ImGuiIO &io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB; // Keyboard mapping. ImGui will use
                                          // those indices to peek into the
                                          // io.KeyDown[] array.
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

  io.RenderDrawListsFn =
      ImGui_ImplGlfwGL3_RenderDrawLists; // Alternatively you can set this to
                                         // NULL and call ImGui::GetDrawData()
                                         // after ImGui::Render() to get the
                                         // same ImDrawData pointer.
  io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
  io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
#ifdef _WIN32
  io.ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif

  if (install_callbacks) {
    glfwSetMouseButtonCallback(window, ImGui_ImplGlfwGL3_MouseButtonCallback);
    glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);
    glfwSetKeyCallback(window, ImGui_ImplGlfwGL3_KeyCallback);
    glfwSetCharCallback(window, ImGui_ImplGlfwGL3_CharCallback);
  }

  return true;
}

void ImGui_ImplGlfwGL3_Shutdown() {
  ImGui_ImplGlfwGL3_InvalidateDeviceObjects();
  ImGui::Shutdown();
}

void ImGui_ImplGlfwGL3_NewFrame() {
  if (!g_FontTexture)
    ImGui_ImplGlfwGL3_CreateDeviceObjects();

  ImGuiIO &io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(g_Window, &w, &h);
  glfwGetFramebufferSize(g_Window, &display_w, &display_h);
  io.DisplaySize = ImVec2((float)w, (float)h);
  io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0,
                                      h > 0 ? ((float)display_h / h) : 0);

  // Setup time step
  double current_time = glfwGetTime();
  io.DeltaTime =
      g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
  g_Time = current_time;

  // Setup inputs
  // (we already got mouse wheel, keyboard keys & characters from glfw callbacks
  // polled in glfwPollEvents())
  if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED)) {
    double mouse_x, mouse_y;
    glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
    io.MousePos = ImVec2((float)mouse_x, (float)mouse_y); // Mouse position in
                                                          // screen coordinates
                                                          // (set to -1,-1 if no
                                                          // mouse / on another
                                                          // screen, etc.)
  } else {
    io.MousePos = ImVec2(-1, -1);
  }

  for (int i = 0; i < 3; i++) {
    io.MouseDown[i] =
        g_MousePressed[i] || glfwGetMouseButton(g_Window, i) !=
                                 0; // If a mouse press event came, always pass
                                    // it as "mouse held this frame", so we
                                    // don't miss click-release events that are
                                    // shorter than 1 frame.
    g_MousePressed[i] = false;
  }

  io.MouseWheel = g_MouseWheel;
  g_MouseWheel = 0.0f;

  // Hide OS mouse cursor if ImGui is drawing it
  glfwSetInputMode(g_Window, GLFW_CURSOR,
                   io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN
                                      : GLFW_CURSOR_NORMAL);

  // Start the frame
  ImGui::NewFrame();
}
