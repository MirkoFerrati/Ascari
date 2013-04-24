#include <QtGui/QApplication>
#include "mainwindow.h"
#include "../shared/communication/global.h"
#include <thread>
int main(int argc, char *argv[])
{

   LOGOG_INITIALIZE();
   std::thread exiting;
   static_zmq::context=new zmq::context_t(1);
   {
    logog::Cout out;
    QApplication a(argc, argv);
    MainWindow w;
	w.setMinimumSize(200,200);
    w.show();

    a.exec();
    exiting=std::thread ( []() {  delete(static_zmq::context);    }     );

    }
    exiting.join();
return 0;
}
