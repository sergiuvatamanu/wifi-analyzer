#include "wifiAnalyzerApp.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    wifiAnalyzerApp w;
    w.show();

    return a.exec();
}
