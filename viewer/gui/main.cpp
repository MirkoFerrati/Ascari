#include <QtGui/QApplication>
#include "viewer.h"
#include "../plugins/agent_router/agent_router_viewer.h"
#include <QtGui/QDesktopWidget>
#include "udp_world_sniffer.h"
#include "../communication/zmq_world_sniffer.hpp"
#include "lemon/arg_parser.h"
#include "logog.hpp"
#include "../gml2lgf/graph.h"
#include "../communication/zmq_identifier_sniffer.hpp"
#include <QSettings>
#include "../../shared/types/monitor_packet.h"
#include "../plugins/task_assignment/task_assignment_viewer.h"
#include "../plugins/monitor/monitor_viewer.h"
#include <ostream>
#include <sstream>
#include "../plugins/addplugins.h"


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
        std::string filename="";
	auto generic_plugins=createPlugins();
        logog::Cout out;
        lemon::ArgParser ap ( argc,argv );
        int viewerType=0;
        ap.refOption ( "f","Yaml filename",filename );
        ap.synonym ( "filename","f" );
	std::ostringstream options;
	int i=1;
	options<<"0-blank ";
	for (auto plugin:generic_plugins)
	{
	  options<<i++<<"-"<<plugin->getType()<<" ";
	}
        ap.refOption ( "t",options.str(),viewerType,true ); //ap.refOption ( "t"," 1-baseball 2-grafi 3-vuoto 4-Task Assignment 5-monitor",viewerType,true );
        ap.throwOnProblems();
        try
        {
            ap.parse();
//             if ( ( viewerType==2 ) && !ap.given ( "f" ) )
//             {
//                 ERR ( "inserire il nome del file%s","" );
//                 return 0;
//             }
        }
        catch ( lemon::ArgParserException const& ex )
        {
            ERR ( "errore nella lettura dei parametri %s",ex.reason() );
            return 0;
        }

        QApplication app ( argc,argv );

        std::vector<abstract_viewer_plugin*> plugins;
        world_sim_packet read;
        std::shared_ptr<std::mutex> read_mutex ( new std::mutex );
        Viewer window ( read,read_mutex,NULL );
	if (viewerType>generic_plugins.size())
	  throw "undefined viewer plugin";
	yaml_parser parser;
	for (auto plugin:generic_plugins)
	{
	  plugin->createParserPlugin();
	  parser.addPlugin(plugin->getParserPlugin());
	}
	Parsed_World* world=0;
	if (filename!="")
	  world=new Parsed_World(parser.parse_file(filename));
	else
	  world=0;
	if (viewerType>0)
	{
	    generic_plugins.at(viewerType+1)->createViewerPlugin(&window,world);
            plugins.push_back ( generic_plugins.at(viewerType+1)->getViewerPlugin() );
            generic_plugins.at(viewerType+1)->getViewerPlugin()->setfather ( &window );
            window.addPlugin ( generic_plugins.at(viewerType+1)->getViewerPlugin() );
	}
	/*
        switch ( viewerType )
        {
        case 1:
        {

        }
        break;
        case 2:
        {
            abstract_viewer_plugin* router_viewer = new agent_router_viewer ( filename );
            plugins.push_back ( router_viewer );
            router_viewer->setfather ( &window );
            window.addPlugin ( router_viewer );

        }
        break;
	  case 5:
        {
            abstract_viewer_plugin* temp=new monitor_viewer ( filename );
            temp->setfather ( &window );
            window.addPlugin ( temp );
            plugins.push_back ( temp );
        }
        break;
        case 3:
        {

        }
        case 4:
        {
            abstract_viewer_plugin* ta_viewer = new task_assignment_viewer ();
            plugins.push_back ( ta_viewer );
            ta_viewer->setfather ( &window );
           
            window.addPlugin ( ta_viewer );
        }
        break;
        default:
            throw "tipo di viewer sconosciuto";
        }
*/
        window.init ( filename );
        zmq_world_sniffer<world_sim_packet> sniffer_world ( read,read_mutex );
        window.setWindowTitle ( "Visualizer" );
        window.show();
        QSettings settings ( "K2BRobotics","Viewer" );
        window.restoreGeometry ( settings.value ( "mainWindowGeometry" ).toByteArray() );
        window.setMinimumSize ( 200,200 );
        sniffer_world.start_receiving();
        window.start();
        app.exec();
// 	std::cout<<"la main window si è chiusa"<<std::endl;
        sniffer_world.stop_receiving();

    for ( auto plugin:plugins )
        {
            delete plugin;
        }

        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );
//     	std::cout<<"contesto chiuso"<<std::endl;
    }
    exiting.join();
    return 0;
}

