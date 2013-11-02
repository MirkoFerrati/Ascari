#include <QtGui/QApplication>
#include "viewer.h"
//#include "../plugins/agent_router/agent_router_viewer.h"
#include <QtGui/QDesktopWidget>
//#include "udp_world_sniffer.h"
#include "../communication/zmq_world_sniffer.hpp"
#include "logog.hpp"
#include "../gml2lgf/graph.h"

#include <QSettings>

//#include "../plugins/task_assignment/task_assignment_viewer.h"
#include <ostream>
#include <sstream>
#include <boost/program_options.hpp>
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
        int viewerType=0;
        
	auto generic_plugins=createPlugins();
        logog::Cout out;

        boost::program_options::options_description desc;
        boost::program_options::variables_map options;
        desc.add_options()("help,h","Get help");
        
        if (CONFIG.exists("FILENAME"))
        {
            filename=CONFIG.getValue("FILENAME");
            desc.add_options()("filename,f",boost::program_options::value<std::string>(&filename), "Yaml filename");
            INFO("Using %s as filename, read from config file",filename.c_str());
        }
        else if (CONFIG.exists("filename"))
        {
            filename=CONFIG.getValue("filename");
            desc.add_options()("filename,f",boost::program_options::value<std::string>(&filename), "Yaml filename");
            INFO("Using %s as filename, read from config file",filename.c_str());
        }    
        else
            desc.add_options()("filename,f",boost::program_options::value<std::string>(&filename)->required(), "Yaml filename");
        
        std::ostringstream options_desc;
	int i=1;
	options_desc<<"0-blank ";
	for (auto plugin:generic_plugins)
	{
	  options_desc<<i++<<"-"<<plugin->getType()<<" ";
	}
        
        desc.add_options()("type,t", boost::program_options::value<int>(&viewerType)->required(),options_desc.str().c_str());
 
        for (auto config_value:CONFIG.getMap())
        {
            if (config_value.first!="filename")
                desc.add_options()(config_value.first.c_str(),config_value.second.data().c_str());
        }
        
        boost::program_options::positional_options_description p;
        p.add("type",-1);
        
        
        try
        {
            boost::program_options::store(boost::program_options::command_line_parser(argc,argv).options(desc).positional(p).run(),options);//Overwrite default file values with shell one
            
            if (options.count("help"))
            {
                std::cout << "Basic Command Line Parameter App" << std::endl 
                << desc << std::endl;
                return 0;
            }
            boost::program_options::notify(options);
        }
        catch (boost::program_options::error& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
            std::cerr << desc << std::endl; 
            return 2;    
        }
        
        QApplication app ( argc,argv );

        std::vector<abstract_viewer_plugin*> plugins;
        world_sim_packet read;
        std::shared_ptr<std::mutex> read_mutex ( new std::mutex );
        Viewer window ( read,read_mutex,NULL );
	if (viewerType>generic_plugins.size())
        {
            std::cout << "Basic Command Line Parameter App" << std::endl << desc << std::endl;
            return 0;
        }
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
	    generic_plugins.at(viewerType-1)->createViewerPlugin(&window,world);
            plugins.push_back ( generic_plugins.at(viewerType-1)->getViewerPlugin() );
            generic_plugins.at(viewerType-1)->getViewerPlugin()->setfather ( &window );
            window.addPlugin ( generic_plugins.at(viewerType-1)->getViewerPlugin() );
	}
        window.init ( filename );
        zmq_world_sniffer<world_sim_packet> sniffer_world ( read,read_mutex );
        window.setWindowTitle ( "Visualizer" );
        window.show();
        QSettings settings ( "Ascari","Viewer" );
        window.restoreGeometry ( settings.value ( "mainWindowGeometry" ).toByteArray() );
        window.setMinimumSize ( 200,200 );
        sniffer_world.start_receiving();
        window.start();
        app.exec();
        sniffer_world.stop_receiving();

    for ( auto plugin:plugins )
        {
            delete plugin;
        }

        exiting=std::thread ( []()
        {
            delete ( static_zmq::context );
        } );
    }
    exiting.join();
    return 0;
}

