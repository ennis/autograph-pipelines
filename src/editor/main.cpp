// std
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <optional.hpp>

#include <gl_core_4_5.h>
#include <GLFW/glfw3.h>
#include <cppformat/format.h>
// core engine stuff
#include "ScriptEngine.h"
#include <autograph/RectTransform.h>
#include <autograph/editor/Application.h>
#include <autograph/editor/Input.h>
#include <autograph/editor/Observable.h>

#include <SkCanvas.h>
#include <SkPaint.h>
#include <SkSurface.h>

#include "ui/UIBuilder.h"
#include "ui/UICanvas.h"
//#include <autograph/editor/Await.h>

GLFWwindow *window;
constexpr auto init_width = 640;
constexpr auto init_height = 480;
constexpr float twopi_f = 6.28318530718f;

class EditorApplication : public ag::Application {
public:
  EditorApplication() : ag::Application{ag::uvec2{640, 480}} { setupUi(); }

  void onInputEvent(ag::InputEvent &ev) override {
    ui.onInput(mainPanel, ev);
    // ui::HUDHandleInput(s, rootPanel, ev);
    if (auto keyEvent = ev.as<ag::KeyEvent>()) {
      if (keyEvent->code == GLFW_KEY_F5 &&
          keyEvent->state == ag::KeyState::Pressed) {
        fmt::print(std::clog, "Reloading assemblies...\n");
      }
    }
  }

  void setupUi() {
    using namespace ag::ui::builder;
    auto &sub = getSubscription();

    auto clickedAButton = []() { fmt::print("You clicked a button\n"); };

    mainPanel.addMany(panel(
        verticalLayout(spacing(3.0f)),
        text("Hello world", textSize(40.0f), autoFit(true)), button("OK"),
        panel(gridLayout(4, 3, horizontalSpacing(3.0f), verticalSpacing(3.0f)),
              button(
                  "1",
                  onClick(sub, []() { fmt::print("You clicked button 1\n"); })),
              button(
                  "2",
                  onClick(sub, []() { fmt::print("You clicked button 2\n"); })),
              button(
                  "3",
                  onClick(sub, []() { fmt::print("You clicked button 3\n"); })),
              button("4", onClick(sub, clickedAButton)),
              button("5", onClick(sub, clickedAButton)),
              button("6", onClick(sub, clickedAButton)),
              button("7", onClick(sub, clickedAButton)),
              button("8", onClick(sub, clickedAButton)),
              button("9", onClick(sub, clickedAButton)), dummy(),
              button("0", onClick(sub, clickedAButton)), dummy())));
  }

  void render() override {
    auto framebufferSize = getFramebufferSize();
    glViewport(0, 0, framebufferSize.x, framebufferSize.y);
    glClearColor(60.f / 255.f, 60.f / 255.f, 168.f / 255.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto surface = getSkSurface();
    auto canvas = surface->getCanvas();
    ui.render(mainPanel, *canvas,
              glm::vec2{(float)framebufferSize.x, (float)framebufferSize.y});
    canvas->flush();
  }

  void resize() override {}

private:
  ag::ScriptEngine scriptEngine;
  ag::ui::UICanvas ui;
  ag::ui::Panel mainPanel;
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// MAIN
int main(int argc, char *argv[]) {
  EditorApplication ea{};
  ea.run();
  return 0;
}
