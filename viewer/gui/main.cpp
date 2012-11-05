#include <QtGui/QApplication>
#include "viewer.h"
#include <QtGui/QDesktopWidget>
#include "udp_world_sniffer.h"
#include "lemon/arg_parser.h"
#include "logog.hpp"


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
    LOGOG_INITIALIZE();
    {
        logog::Cout out;
//         if (argc<2)
//             std::cout<<"inserire il tipo di visualizzazione: 1-baseball 2-grafi 3-vuoto"<<std::endl;
        lemon::ArgParser ap(argc,argv);
        std::string graphName;
		int viewerType;
        ap.refOption("f","Graph filename",graphName);
        ap.synonym("filename","f");
		ap.refOption("t"," 1-baseball 2-grafi 3-vuoto",viewerType,true);
        ap.throwOnProblems();
        try {
            ap.parse();
			if (viewerType==2 && !ap.given("f"))
			{
				ERR("inserire il nome del file%s","");
				return 0;
			}
        }
        catch (lemon::ArgParserException const& ex) {
            ERR("errore nella lettura dei parametri %s",ex.reason());
            return 0;
        }
      
        QApplication app(argc,argv);
        boost::asio::io_service io_service;
        std::vector<char> buffer;
        buffer.resize(MAX_PACKET_LENGTH);
        Viewer window(buffer,io_service,NULL,viewerType,graphName);
        udp_world_sniffer sniffer(buffer,io_service);
        window.setWindowTitle("Visualizer");
        window.show();
        center(window);
        sniffer.start_receiving();
        window.start();

        return app.exec();
    }
}
