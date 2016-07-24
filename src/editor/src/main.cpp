// autograph
#include "draw.hpp"
#include "texture.hpp"
#include "vertex_array.hpp"
// node editor
#include "node_editor/graphicsnode.hpp"
#include "node_editor/graphicsnodescene.hpp"
#include "node_editor/graphicsnodesocket.hpp"
#include "node_editor/graphicsnodeview.hpp"
// Qt stuff
#include <QOpenGLWidget>
#include <QtWidgets>
#include <cppformat/format.h>
#include <iostream>

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
  }

  void initializeGL() override { 
	  fmt::print(std::cerr, "initializeGL\n"); 
	  // initialize autograph
	  ag::device_config cfg;
	  cfg.init_fb_width = 640;
	  cfg.init_fb_height = 480;
	  cfg.max_frames_in_flight = 3;
	  ag::initialize(cfg);
	  // VAO: 2D vertices
	  default_vao.initialize({});
	  vao.initialize({
		  ag::vertex_attribute{ 0, GL_FLOAT, 2, 2 * sizeof(float), false } });
	  // shader program
	  prog2d = ag::program::create(VS, FS);
  }

  void resizeGL(int w, int h) override {
    fmt::print(std::cerr, "resizeGL {} {}\n", w, h);
	// reset framebuffer
    //fbo = {};
	// re-create textures
    //tex = ag::texture::create_2d(w, h, ag::image_format::rgba8_unorm);
    //ag::clear(tex, glm::vec4{0.0f, 1.0f, 1.0f, 1.0f});
    //depth_tex = ag::texture::create_2d(w, h, ag::image_format::depth32_float);
    //ag::clear_depth(depth_tex, 1.0f);
	// re-create framebuffer
    //fbo = ag::framebuffer::create({&tex}, depth_tex);
  }

  void paintGL() override {
    fmt::print(std::cerr, "paintGL\n");
	// vertex and index data (a rectangle)
    static const std::array<glm::vec2, 4> vertices{{
        {-0.5f, 0.5f}, {0.5f, 0.5f}, {-0.5f, -0.5f}, {0.5f, -0.5f},
    }};
    static const std::array<int, 6> indices{{0, 1, 2, 2, 1, 3}};
    // upload vertices to the ring buffer
    auto vbo = ag::upload_frame_array(vertices);
    // upload indices
    auto ibo = ag::upload_frame_array(indices);
	// setup transform
	glm::mat4 transform = glm::mat4{ 1.0f };
	// draw stuff
	// this call will unfold into the corresponding glUniform*, glBindVertexBuffers, and glDraw* calls
    ag::draw(defaultFramebufferObject(), prog2d,
             ag::draw_indexed{GL_TRIANGLES, 0, 6, 0, ibo, GL_UNSIGNED_INT},
			// Resources
		     vao,
             ag::vertex_buffer{0, vbo, sizeof(glm::vec2)},
			 ag::uniform_matrix4{ "transform", transform },
			// We have to specify the viewport because we are binding the framebuffer 
		    // from a GLuint, which does not set a default viewport
			ag::viewport{ 0, {0, 0, width(), height()} }
			);
	ag::end_frame();
  }

private:
	ag::vertex_array default_vao;
  ag::vertex_array vao;
  ag::program prog2d;
  ag::texture tex;
  ag::texture depth_tex;
  ag::framebuffer fbo;
};

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow() {
    setDockNestingEnabled(true);
    // setDockOptions(QMainWindow::DockOption::)
    auto nodeViewDock = new QDockWidget(tr("Node view"));
    nodeViewScene = new GraphicsNodeScene(this);
    nodeViewScene->setSceneRect(-32000, -32000, 64000, 64000);
    GraphicsNodeView *nodeView = new GraphicsNodeView(nodeViewScene);
    nodeView->setAlignment(Qt::AlignCenter);
    nodeViewDock->setWidget(nodeView);
    addDockWidget(Qt::RightDockWidgetArea, nodeViewDock);

    auto entityViewDock = new QDockWidget(tr("Scene"));
    entityViewDock->setWidget(new QWidget());
    addDockWidget(Qt::LeftDockWidgetArea, entityViewDock);

    //auto openglViewDock = new QDockWidget(tr("Scene View"));
    //openglViewDock->setWidget(new OpenGLWidget());
    //addDockWidget(Qt::RightDockWidgetArea, openglViewDock);
	auto glw = new OpenGLWidget();
	glw->show();

    createMenus();
    createActions();
    populate();
  }

  void populate() {
    for (int i = 0; i < 5; i++) {
      auto n = new GraphicsNode();
      for (int j = i; j < 5; j++) {
        n->setPos(i * 25, i * 25);
        auto sink = n->add_sink("sink");
        auto source = n->add_source("source");
      }
      if (i == 4) {
        QTextEdit *te = new QTextEdit();
        te->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        te->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        n->setCentralWidget(te);
      }
      n->setTitle(QString("GraphicsNode %1").arg(i));
      nodeViewScene->addItem(n);
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

  QGraphicsScene *nodeViewScene = nullptr;
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
