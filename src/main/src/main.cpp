// Qt stuff
#include <experimental/filesystem>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <autograph/gl/Device.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/gl/Texture.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>

#include "Effect.h"

#include <QtWidgets>

using namespace ag;

std::string pathCombine(std::string a, std::string b) {
  std::experimental::filesystem::path p{std::move(a)};
  p /= std::move(b);
  return p.string();
}

sol::state* gLuaState = nullptr;

class OpenGLWidget : public QOpenGLWidget {
public:
  OpenGLWidget() : QOpenGLWidget{nullptr, Qt::Window} {
    AG_DEBUG("ctor OpenGLWidget");
    QSurfaceFormat fmt;
    fmt.setVersion(4, 5);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
    setFormat(fmt);
    // check for a 4.5 core profile context
    auto f = format();
    AG_DEBUG("OpenGL context version: {}.{}", f.version().first,
             f.version().second);
    if (f.profile() != QSurfaceFormat::CoreProfile) {
      AG_DEBUG("OpenGL context is not a core profile context.");
    }
  }

  void initializeGL() override {
    AG_DEBUG("initializeGL");
    if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED) {
      ag::failWith("OpenGL init failed(ogl_LoadFunctions).");
    }

    gl::DeviceConfig devcfg;
    devcfg.init_fb_width = width();
    devcfg.init_fb_height = height();
    devcfg.max_frames_in_flight = 3;
    gl::initialize(devcfg);

	reloadPipeline();
  }

  void reloadPipeline() {
    AG_DEBUG("==============================================");
    AG_DEBUG("Reloading pipelines");
    auto& lua = *gLuaState;
    lua.require("gl", sol::c_call<decltype(&openLuaModule_GL), &openLuaModule_GL>);
    lua.script_file(getActualPath("resources/scripts/init.lua"));
  }

  void resizeGL(int w, int h) override {
    AG_DEBUG("resizeGL {} {}", w, h);
    gl::resizeDefaultFramebuffer(ivec2{w, h});
  }

  void paintGL() override {
    AG_DEBUG("paintGL");
    // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    // glClearDepth(1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

private:
  std::unique_ptr<DrawPass> testpass;
};

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow() {
    setDockNestingEnabled(true);
    // setDockOptions(QMainWindow::DockOption::)

    sceneView = new OpenGLWidget();

    auto entityViewDock = new QDockWidget(tr("Scene"));
    entityViewDock->setWidget(new QWidget());
    addDockWidget(Qt::LeftDockWidgetArea, entityViewDock);

    auto openglViewDock = new QDockWidget(tr("Scene View"));
    openglViewDock->setWidget(sceneView);
    addDockWidget(Qt::RightDockWidgetArea, openglViewDock);
    // auto glw = new OpenGLWidget();
    // glw->show();
    sceneView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(sceneView, &QWidget::customContextMenuRequested,
            [this](const QPoint &pos) { this->showSceneViewContextMenu(pos); });

    createMenus();
    createActions();
  }

  void showSceneViewContextMenu(const QPoint &pos) // this is a slot
  {
    // for most widgets
    QPoint globalPos = sceneView->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    QAction *act = new QAction("Menu Item 1", nullptr);
    myMenu.addAction(act);

    QAction *selectedItem = myMenu.exec(globalPos);
    if (selectedItem == act) {
      AG_DEBUG("Do something!");
      // something was chosen, do stuff
    } else {
      // nothing was chosen
    }
  }

private slots:

private:
  void createMenus() {
    projectMenu = menuBar()->addMenu("Project");
    editMenu = menuBar()->addMenu("Edit");
    createMenu = menuBar()->addMenu("Create");
    createComponentMenu = createMenu->addMenu("Add component");
  }

  void createActions() {
    // Project menu
    openAction = new QAction(tr("&Open project..."), this);
    openAction->setShortcut(QKeySequence::New);
    closeAction = new QAction(tr("Close project"), this);
    closeAction->setShortcut(tr("Ctrl+F4"));
    reloadShadersAction = new QAction(tr("Reload shaders"), this);
    reloadShadersAction->setShortcut(tr("Ctrl+F5"));
    connect(openAction, &QAction::triggered, this,
            [this]() { AG_DEBUG("Open project"); });
    connect(closeAction, &QAction::triggered, this,
            [this]() { AG_DEBUG("Close project"); });
    connect(reloadShadersAction, &QAction::triggered, this,
            [this]() { this->sceneView->reloadPipeline(); });
    projectMenu->addAction(openAction);
    projectMenu->addAction(closeAction);
    projectMenu->addAction(reloadShadersAction);

    // Edit menu
    undoAction = new QAction(tr("Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction = new QAction(tr("Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);

    // Create menu
    createEntityAction = new QAction(tr("Create entity"), this);
    createEntityAction->setShortcut(QKeySequence("Ctrl+Shift+N"));
    addTransformComponentAction = new QAction(tr("Transform"), this);
    addComponentAction = new QAction(tr("Component..."), this);
    connect(createEntityAction, &QAction::triggered,
            [this]() { AG_DEBUG("Create entity"); });
    connect(addTransformComponentAction, &QAction::triggered,
            [this]() { AG_DEBUG("Add transform component"); });
    connect(addComponentAction, &QAction::triggered,
            [this]() { AG_DEBUG("Add component"); });
    createMenu->addAction(createEntityAction);
    createComponentMenu->addAction(addComponentAction);
    createComponentMenu->addSeparator();
    createComponentMenu->addAction(addTransformComponentAction);
  }

  OpenGLWidget *sceneView = nullptr;

  QMenu *projectMenu = nullptr;
  QMenu *editMenu = nullptr;
  QMenu *createMenu = nullptr;
  QMenu *createComponentMenu = nullptr;

  QAction *undoAction = nullptr;
  QAction *redoAction = nullptr;
  QAction *openAction = nullptr;
  QAction *closeAction = nullptr;
  QAction *reloadShadersAction = nullptr;
  QAction *createEntityAction = nullptr;
  QAction *addTransformComponentAction = nullptr;
  QAction *addComponentAction = nullptr;
};

int main(int argc, char *argv[]) {
  // Q_INIT_RESOURCE(textfinder);
  QSurfaceFormat fmt;
  fmt.setVersion(4, 5);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setOption(QSurfaceFormat::DebugContext);
  QSurfaceFormat::setDefaultFormat(fmt);
  QApplication app(argc, argv);
  // initialize Lua VM
  sol::state luaState;
  luaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::ffi,
	  sol::lib::jit, sol::lib::string, sol::lib::io,
	  sol::lib::math);
  gLuaState = &luaState;

  QFile styleSheetFile(":qdarkstyle/style.qss");
  if (!styleSheetFile.exists())
    ag::warningMessage("Unable to set stylesheet, file not found");
  else {
    styleSheetFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&styleSheetFile);
    app.setStyleSheet(ts.readAll());
  }

  for (auto &&s : app.libraryPaths())
    AG_DEBUG("Qt library path: {}", s.toStdString().c_str());
  MainWindow w;
  w.show();
  return app.exec();
}

#include "main.moc"
