#include <QtWidgets>
#include <iostream>
#include <cppformat/format.h>

class MainWindow : public QMainWindow 
{
	Q_OBJECT
public:
private:
};

int main(int argc, char *argv[])
{
	//Q_INIT_RESOURCE(textfinder);
	QApplication app(argc, argv);
	fmt::print(std::cerr, "FUCK YOU\n");
	for (auto&& s : app.libraryPaths()) 
		fmt::print(std::cerr, "Qt library path: {}\n", s.toStdString().c_str());
		MainWindow w;
	w.show();
	return app.exec();
}

#include "main.moc"