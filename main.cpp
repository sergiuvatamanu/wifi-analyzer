#include "WifiAnalyzer.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WifiAnalyzer w;
    w.show();

    return a.exec();
}
