#include "ImplicitStorage.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImplicitStorage w;
    w.show();
    return a.exec();
}
