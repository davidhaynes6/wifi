#include "wifi.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    wifi w;
    w.show();
    return a.exec();
}
