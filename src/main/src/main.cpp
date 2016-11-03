// Qt stuff
#include <cppformat/format.h>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <experimental/filesystem>

#include <autograph/gl/Device.h>
#include <autograph/gl/Texture.h>
#include <autograph/gl/Framebuffer.h>
#include <autograph/gl/Program.h>
#include <autograph/support/ProjectRoot.h>

#include <QtWidgets>

using namespace ag;

const char* VS = R"(
#version 440
layout(location=0) in vec2 position;
uniform mat4 transform;
void main() {
	vec4 P = vec4(position, 0.0, 1.0);
	gl_Position = transform * P;
}
)";

const char* FS = R"(
#version 440
layout(location=0) out vec4 color;
void main() {
	color = vec4(0.0, 1.0, 0.0, 1.0);
}
)";

std::string pathCombine(std::string a, std::string b)
{
	std::experimental::filesystem::path p{ std::move(a) };
	p /= std::move(b);
	return p.string();
}

class OpenGLWidget : public QOpenGLWidget {
public:
  OpenGLWidget() : QOpenGLWidget{ nullptr, Qt::Window } {
	fmt::print(std::cerr, "ctor OpenGLWidget\n");
    QSurfaceFormat fmt;
    fmt.setVersion(4, 5);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
    setFormat(fmt);
	// check for a 4.5 core profile context
	auto f = format();
	fmt::print(std::cerr, "OpenGL context version: {}.{}\n", f.version().first, f.version().second);
	if (f.profile() != QSurfaceFormat::CoreProfile) {
		fmt::print(std::cerr, "OpenGL context is not a core profile context.\n");
	}

	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED) {
		//fmt::print(std::cerr, "OpenGL init failed (ogl_LoadFunctions).\n");
		throw std::runtime_error("OpenGL init failed (ogl_LoadFunctions).\n");
	}

	gl::DeviceConfig devcfg;
	devcfg.init_fb_width = width();
	devcfg.init_fb_height = height();
	devcfg.max_frames_in_flight = 3;
	ag::gl::initialize(devcfg);

	namespace fs = std::experimental::filesystem;

	const auto& rootPath = getProjectRootDirectory();
	prog = gl::Program::loadFromFile(pathCombine(rootPath, "resources/glsl/blur.glsl").c_str(), gl::ShaderStage::Compute, { {} });
  }

  void initializeGL() override { 
	  fmt::print(std::cerr, "initializeGL\n"); 
  }

  void resizeGL(int w, int h) override {
    fmt::print(std::cerr, "resizeGL {} {}\n", w, h);
	gl::resizeDefaultFramebuffer(ivec2{ w,h });
  }

  void paintGL() override {
    fmt::print(std::cerr, "paintGL\n");
	//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	//glClearDepth(1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

private:
	gl::Program prog;
};

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow() {
    setDockNestingEnabled(true);
    // setDockOptions(QMainWindow::DockOption::)

    auto entityViewDock = new QDockWidget(tr("Scene"));
    entityViewDock->setWidget(new QWidget());
    addDockWidget(Qt::LeftDockWidgetArea, entityViewDock);

    auto openglViewDock = new QDockWidget(tr("Scene View"));
    openglViewDock->setWidget(new OpenGLWidget());
    addDockWidget(Qt::RightDockWidgetArea, openglViewDock);
	//auto glw = new OpenGLWidget();
	//glw->show();

    createMenus();
    createActions();
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
    connect(openAction, &QAction::triggered, this,
            [this]() { fmt::print(std::cerr, "Open project\n"); });
    connect(closeAction, &QAction::triggered, this,
            [this]() { fmt::print(std::cerr, "Close project\n"); });
    projectMenu->addAction(openAction);
    projectMenu->addAction(closeAction);

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
            [this]() { fmt::print(std::cerr, "Create entity\n"); });
    connect(addTransformComponentAction, &QAction::triggered,
            [this]() { fmt::print(std::cerr, "Add transform component\n"); });
    connect(addComponentAction, &QAction::triggered,
            [this]() { fmt::print(std::cerr, "Add component\n"); });
    createMenu->addAction(createEntityAction);
    createComponentMenu->addAction(addComponentAction);
    createComponentMenu->addSeparator();
    createComponentMenu->addAction(addTransformComponentAction);
  }

  QMenu *projectMenu = nullptr;
  QMenu *editMenu = nullptr;
  QMenu *createMenu = nullptr;
  QMenu *createComponentMenu = nullptr;

  QAction *undoAction = nullptr;
  QAction *redoAction = nullptr;
  QAction *openAction = nullptr;
  QAction *closeAction = nullptr;
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

  QFile styleSheetFile(":qdarkstyle/style.qss");
  if (!styleSheetFile.exists())
    fmt::print("Unable to set stylesheet, file not found\n");
  else {
    styleSheetFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&styleSheetFile);
    app.setStyleSheet(ts.readAll());
  }

  fmt::print(std::cerr, "FUCK YOU\n");
  for (auto &&s : app.libraryPaths())
    fmt::print(std::cerr, "Qt library path: {}\n", s.toStdString().c_str());
  MainWindow w;
  w.show();
  return app.exec();
}

#include "main.moc"
