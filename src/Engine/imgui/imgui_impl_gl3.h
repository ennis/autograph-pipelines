// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture
// identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See
// main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions:
// ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and
// ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top
// of imgui.cpp.
// https://github.com/ocornut/imgui

namespace ag {
class Window;
struct Event;
bool ImGui_Impl_Init();
void ImGui_Impl_Shutdown();
void ImGui_Impl_NewFrame(Window &window, double current_time);
// Use if you want to reset your rendering device without losing ImGui state.
void ImGui_Impl_InvalidateDeviceObjects();
bool ImGui_Impl_CreateDeviceObjects();
// Call this when an event is received
void ImGui_Impl_ProcessEvent(const Event &e);
}

// GLFW callbacks (installed by default if you enable 'install_callbacks' during
// initialization)
// Provided here if you want to chain callbacks.
// You can also handle inputs yourself and use those as a reference.
/*IMGUI_API void        ImGui_ImplGL3_MouseButtonCallback(GLFWwindow* window,
int button, int action, int mods);
IMGUI_API void        ImGui_ImplGL3_ScrollCallback(GLFWwindow* window, double
xoffset, double yoffset);
IMGUI_API void        ImGui_ImplGL3_KeyCallback(GLFWwindow* window, int key, int
scancode, int action, int mods);
IMGUI_API void        ImGui_ImplGL3_CharCallback(GLFWwindow* window, unsigned
int c);*/
