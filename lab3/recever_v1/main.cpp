#include "recever_v1.h"
#include <QtWidgets/QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	recever_v1 w;
	w.show();
	return a.exec();
}