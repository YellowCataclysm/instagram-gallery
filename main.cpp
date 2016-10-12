#include "GalleryWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GalleryWindow w;
	w.show();

	return a.exec();
}
