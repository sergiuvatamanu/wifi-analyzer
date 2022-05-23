#include "WifiAnalyzer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WifiAnalyzer w;
    w.show();

    return a.exec();
}
