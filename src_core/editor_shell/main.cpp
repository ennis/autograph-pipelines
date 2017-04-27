#include <autograph/engine/All.h>
#include <autograph/gl/All.h>
#include <QtWidgets>
#include <autograph/editor/QtExtension.h>

using namespace ag;

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

  void initializeGL() override { AG_DEBUG("initializeGL"); }
  void resizeGL(int w, int h) override { AG_DEBUG("resizeGL {} {}", w, h); }
  void paintGL() override { AG_DEBUG("paintGL"); }

private:
};

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow() {

    setDockNestingEnabled(true);
    // setDockOptions(QMainWindow::DockOption::)
    auto nodeViewDock = new QDockWidget(tr("Node view"));
    addDockWidget(Qt::RightDockWidgetArea, nodeViewDock);

    auto entityViewDock = new QDockWidget(tr("Scene"));
    entityViewDock->setWidget(new QWidget());
    addDockWidget(Qt::LeftDockWidgetArea, entityViewDock);

    auto openglViewDock = new QDockWidget(tr("Scene View"));
    openglViewDock->setWidget(new OpenGLWidget());
    addDockWidget(Qt::RightDockWidgetArea, openglViewDock);

    createMenus();
    createActions();

	curveEditor = createClassInstance<QtExtension>("CurveEditor");
	if (curveEditor)
		curveEditor->initialize(this);
  }

private Q_SLOTS:

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
            [this]() { AG_DEBUG("Open project"); });
    connect(closeAction, &QAction::triggered, this,
            [this]() { AG_DEBUG("Close project"); });
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

  PluginPtr<QtExtension> curveEditor;
};

int main(int argc, char *argv[]) {
	// load plugins
	loadPluginModule("CurveEditor");

  // Q_INIT_RESOURCE(textfinder);
  QApplication app(argc, argv);
  //QApplication::setStyle(QStyleFactory::create("Fusion"));

  QFile styleSheetFile(":qdarkstyle/style.qss");
  if (!styleSheetFile.exists())
    AG_DEBUG("Unable to set stylesheet, file not found");
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
