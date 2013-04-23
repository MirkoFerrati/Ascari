#include <QtGui/QApplication>
#include "viewer.h"
#include "agent_router_viewer.h"
#include <QtGui/QDesktopWidget>
#include "udp_world_sniffer.h"
#include "../communication/zmq_world_sniffer.hpp"
#include "lemon/arg_parser.h"
#include "logog.hpp"
#include "../gml2lgf/graph.h"
#include "../communication/zmq_identifier_sniffer.hpp"
#include <QSettings>
#include "../../shared/types/monitor_packet.h"

void center ( QWidget &widget,int WIDTH=800,int HEIGHT=800 )
{
    int x, y;
    int screenWidth;
    int screenHeight;

    QDesktopWidget *desktop = QApplication::desktop();

    screenWidth = desktop->width();
    screenHeight = desktop->height();

    x = ( screenWidth - WIDTH ) / 2;
    y = ( screenHeight - HEIGHT ) / 2;

    widget.setGeometry ( x, y, WIDTH, HEIGHT );

}


int main ( int argc, char *argv[] )
{
    LOGOG_INITIALIZE();
    std::thread exiting;
    static_zmq::context=new zmq::context_t ( 1 );
    {
        //written by Alessandro Settimi

        std::string filename;
        //written by Alessandro Settimi

        logog::Cout out;
//         if (argc<2)
//             std::cout<<"inserire il tipo di visualizzazione: 1-baseball 2-grafi 3-vuoto"<<std::endl;
        lemon::ArgParser ap ( argc,argv );
        int viewerType=0;

        ap.refOption ( "f","Yaml filename",filename );

        ap.synonym ( "filename","f" );
        ap.refOption ( "t"," 1-baseball 2-grafi 3-vuoto 4-Task Assignment 5-monitor",viewerType,true );

        ap.throwOnProblems();
        try
        {
            ap.parse();
            if ( ( viewerType==2 || viewerType==4 ) && !ap.given ( "f" ) )
            {
                ERR ( "inserire il nome del file%s","" );
                return 0;
            }
        }
        catch ( lemon::ArgParserException const& ex )
        {
            ERR ( "errore nella lettura dei parametri %s",ex.reason() );
            return 0;
        }


        QApplication app ( argc,argv );
        std::unique_ptr<world_sniffer_abstract> identifier_sniffer;
	std::vector<viewer_plugin*> plugins;
	world_sim_packet read;
        std::shared_ptr<std::mutex> read_mutex ( new std::mutex );
        Viewer window ( read,read_mutex,NULL );
        switch ( viewerType )
        {
        case 1:
        {

        }
        break;
        case 2:
        {
            boost::asio::io_service io_service;
            std::vector<char> buffer;
            buffer.resize ( MAX_PACKET_LENGTH );
            udp_world_sniffer sniffer ( buffer,io_service );
	    viewer_plugin* router_viewer = new agent_router_viewer();
	    plugins.push_back(router_viewer);
	    router_viewer->setfather(&window);
	    window.addPlugin(router_viewer);

        }
        break;
        case 3:
        {

        }
        case 4:
        {

        }
        case 5:
        {
            std::map<std::string,monitor_packet> monitor_read;
            std::shared_ptr<std::mutex> monitor_read_mutex ( new std::mutex );

            identifier_sniffer=std::unique_ptr<zmq_identifier_sniffer> ( new zmq_identifier_sniffer ( monitor_read,monitor_read_mutex ) );
            identifier_sniffer->start_receiving();

            //TODO:window.setMonitor(&monitor_read,monitor_read_mutex);

        }
        break;
        default:
            throw "tipo di viewer sconosciuto";
        }


 
        //TODO:create plugins
        window.init ( filename );
        zmq_world_sniffer<world_sim_packet> sniffer_world ( read,read_mutex );

        window.setWindowTitle ( "Visualizer" );
        window.show();

        QSettings settings ( "K2BRobotics","Viewer" );

        window.restoreGeometry ( settings.value ( "mainWindowGeometry" ).toByteArray() );

        sniffer_world.start_receiving();
        window.start();

        app.exec();
// 	std::cout<<"la main window si è chiusa"<<std::endl;
        if ( identifier_sniffer )
            identifier_sniffer->stop_receiving();
        sniffer_world.stop_receiving();

	for (auto plugin:plugins)
	{
	  delete plugin;
	}
	
        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );
//	delete(static_zmq::context);//.~context_t();


//     	std::cout<<"contesto chiuso"<<std::endl;

    }
//     	std::cout<<"la main program si è chiusa"<<std::endl;
    exiting.join();
    return 0;
}
