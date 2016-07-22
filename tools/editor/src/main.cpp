#include "node_editor/graphicsnodeview.hpp"
#include "node_editor/graphicsnodescene.hpp"
#include "node_editor/graphicsnode.hpp"
#include <QtWidgets>
#include <cppformat/format.h>
#include <iostream>

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow() {
    QWidget *widget = new QWidget;
    setCentralWidget(widget);
    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QVBoxLayout *layout = new QVBoxLayout;
    // layout->setMargin(1);

    nodeViewScene = new GraphicsNodeScene(this);
    nodeViewScene->setSceneRect(-32000, -32000, 64000, 64000);
    GraphicsNodeView *nodeView = new GraphicsNodeView(nodeViewScene);
    nodeView->setAlignment(Qt::AlignCenter);

    layout->addWidget(topFiller);
    layout->addWidget(nodeView);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    createMenus();
    createActions();
    populate();
  }

  void populate() {
    for (int i = 0; i < 5; i++) {
      auto n = new GraphicsNode();
      for (int j = i; j < 5; j++) {
        n->setPos(i * 25, i * 25);
        n->add_sink("sink");
        n->add_source("source");
      }
      if (i == 4) {
        QTextEdit *te = new QTextEdit();
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
