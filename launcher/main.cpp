#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

   LOGOG_INITIALIZE();
   {
    logog::Cout out;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
    }
}
