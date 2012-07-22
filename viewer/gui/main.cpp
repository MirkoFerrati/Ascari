#include <QtGui/QApplication>
#include "viewer.h"
#include <QtGui/QDesktopWidget>
#include "udp_world_sniffer.h"



void center(QWidget &widget,int WIDTH=800,int HEIGHT=800)
{
  int x, y;
  int screenWidth;
  int screenHeight;

  QDesktopWidget *desktop = QApplication::desktop();

  screenWidth = desktop->width();
  screenHeight = desktop->height();

  x = (screenWidth - WIDTH) / 2;
  y = (screenHeight - HEIGHT) / 2;

  widget.setGeometry(x, y, WIDTH, HEIGHT);

}


int main(int argc, char *argv[])
{
	if (argc<2)
		std::cout<<"inserire il tipo di visualizzazione: 1-baseball 2-grafi 3-vuoto"<<std::endl;
    QApplication app(argc,argv);
	boost::asio::io_service io_service;
	std::vector<char> buffer;
	buffer.resize(1000);
    Viewer window(buffer,io_service,NULL,atoi(argv[1]));
	udp_world_sniffer sniffer(buffer,io_service);
   window.setWindowTitle("Visualizer");
   window.show();
   center(window);
  sniffer.start_receiving();
  window.start();

   return app.exec();
}
