#include <autograph/engine/Application.h>
#include <autograph/engine/Input.h>

#include <gl_core_4_5.h>
#include <GLFW/glfw3.h>
#include <GrContext.h>
#include <SkCanvas.h>
#include <SkSurface.h>
#include <stdexcept>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

namespace ag {

// Private API of Input.cpp
void initializeInput(GLFWwindow *window);
void processInput();

static Application* gApplication{ nullptr };

void Application::appWindowSizeChanged(GLFWwindow* window, int width, int height)
{
	if (gApplication)
	gApplication->handleWindowSizeChanged(ivec2{ (unsigned)width, (unsigned)height });
}

struct Application::Impl {
  Subscription subscription;
  GLFWwindow *window;
  GrContext *skiaGrContext;
  sk_sp<SkSurface> skiaSurface;
};

Application::Application(ivec2 initSize) : impl{std::make_unique<Impl>()} {
  initialize(initSize);
}

Application::~Application() { terminate(); }

void Application::terminate() {
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();
}

GLFWwindow *Application::getWindow() const { return impl->window; }

Subscription &Application::getSubscription() { return impl->subscription; }

void Application::resize(ivec2 size) {}

void Application::render() {}

void Application::onInputEvent(InputEvent &ev) {}

ivec2 Application::getFramebufferSize() {
  int w, h;
  glfwGetFramebufferSize(impl->window, &w, &h);
  return ivec2{w, h};
}

ivec2 Application::getWindowSize() {
  int w, h;
  glfwGetWindowSize(impl->window, &w, &h);
  return ivec2{w, h};
}

SkSurface *Application::getSkSurface() { return impl->skiaSurface.get(); }

void Application::run() {
  while (!glfwWindowShouldClose(impl->window)) {
    // trigger input polling
    processInput();
    ImGui_ImplGlfwGL3_NewFrame();
    render();
    ImGui::Render();
    //ag::gl::endFrame();
    glfwSwapBuffers(impl->window);
    glfwPollEvents();
  }
}

void Application::handleWindowSizeChanged(ivec2 newSize)
{
	//createSkiaSurface(newSize);
	resize(newSize);
}

void Application::createSkiaSurface(ivec2 size)
{
	GrBackendRenderTargetDesc desc;
	desc.fWidth = (int)size.x;
	desc.fHeight = (int)size.y;
	desc.fRenderTargetHandle = 0;
	desc.fOrigin = kBottomLeft_GrSurfaceOrigin;
	desc.fConfig = kBGRA_8888_GrPixelConfig;
	desc.fSampleCnt = 8;
	desc.fStencilBits = 8;
	impl->skiaSurface = SkSurface::MakeFromBackendRenderTarget(
	impl->skiaGrContext, desc, nullptr);
}

void Application::initialize(ivec2 initSize) {
  // Initialize the library
  if (!glfwInit())
    throw std::runtime_error("Application failed to initialize (glfwInit)");

  // Create a windowed mode window and its OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  impl->window =
      glfwCreateWindow(initSize[0], initSize[1], "Render", NULL, NULL);
  if (!impl->window) {
    glfwTerminate();
    throw std::runtime_error(
        "Application failed to initialize (glfwCreateWindow)");
  }

  glfwMakeContextCurrent(impl->window);
  if (!ogl_LoadFunctions()) {
	  glfwTerminate();
	  throw std::runtime_error(
		  "Application failed to initialize (ogl_LoadFunctions)");
  }
  initializeInput(impl->window);

  gApplication = this;

  glfwSetWindowSizeCallback(impl->window, appWindowSizeChanged);

  // autograph init
  /*ag::DeviceConfig devCfg;
  devCfg.init_fb_width = width;
  devCfg.init_fb_height = height;
  devCfg.max_frames_in_flight = 3;
  ag::initialize(devCfg);*/

  // ImGui init
  ImGui_ImplGlfwGL3_Init(impl->window, false);

  // Skia init
  impl->skiaGrContext = GrContext::Create(GrBackend::kOpenGL_GrBackend, 0);
  createSkiaSurface(initSize);
 
  // subscribe to input events
  ag::onInputEvent.subscribe(impl->subscription,
                                [this](auto ev) { this->onInputEvent(*ev); });
}
}