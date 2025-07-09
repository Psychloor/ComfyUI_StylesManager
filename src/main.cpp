#define WIN32_LEAN_AND_MEAN


#include <QApplication>

#include "include/main_window.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setStyle("Fusion");
	QApplication::setApplicationName("Prompt Styles Manager");
	QApplication::setApplicationVersion("1.0");

	main_window window;
	window.setMinimumSize(600, 400); // Set a reasonable minimum size
	window.resize(800, 600); // Set the initial window size
	window.show();

	return QApplication::exec();
}
