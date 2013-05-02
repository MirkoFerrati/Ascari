#include "agent_router.h"
#include <lemon/dijkstra.h>
#include <lemon/path.h>
#include <lemon/random.h>
#include <time.h>
#include <agent.h>
#include <lemon/adaptors.h>
#include <cassert>

#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dijkstra.h>
#include <lemon/concepts/maps.h>
#include <sstream>
#include <iomanip>
#include<lemon/graph_to_eps.h>
#include "debug_constants.h"
#include "graph_creator.h"


using namespace std;


void agent_router::print_state(state s)
{
	stringstream os;
		switch( s )
		{
			case state::MOVING: os << "moving"; break;
			case state::LISTENING: os << "listening"; break;
			case state::LOADING: os << "loading"; break;
			case state::ARC_HANDSHAKING: os << "arc_handshaking"; break;
			case state::NODE_HANDSHAKING: os << "node_handshaking"; break;
			case state::EMERGENCY: os << "emergency"; break;
			case state::STARTING: os << "starting"; break;
			case state::STOPPED: os << "stopped"; break;
			default: assert(0);
		}
		cout<<os.str();
	
}

void agent_router::addReservedVariables(exprtk::symbol_table< double >& symbol_table)
{
	symbol_table.add_variable("XTARGET",xtarget,false);
	symbol_table.add_variable("YTARGET",ytarget,false);
	symbol_table.add_variable("V_ROUTER",speed,false);
}


void agent_router::compileExpressions(exprtk::symbol_table< double >& symbol_table)
{
	exprtk::parser<double> parser;
	
	distance_to_target.register_symbol_table(symbol_table);
	if (!parser.compile("sqrt((XTARGET-X)^2+(YTARGET-Y)^2)",distance_to_target))
	{
		ERR("impossibile creare l'espressione: %s %s","sqrt(sqr(XTARGET)+sqr(YTARGET))",parser.error().c_str());
// 		throw "impossibile creare l'espressione";
	}
	
}

int agent_router::findAge(simulation_time present_time, simulation_time old_time)
{
	return round ( ( round ( present_time * 1000.0 ) - round ( old_time * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
}

void agent_router::print_path()
{
    int j=0;
     std::cout<<time<<": path calcolata= ";
    for ( unsigned int i=0; i<node_id.size(); i++ )
    {
        j++;
        if ( j>4 )
            break;
       cout << node_id[i]  << "(" <<  node_id[i]  % graph_node_size << ")" << ">>";
    }
    cout<<endl;
   // std::cout << " next_time=" << next_time << std::endl;

}

void agent_router::stopAgent()
{
	events.at(events_to_index.at("STOPPED"))=(Events::_TRUE);
	events.at(events_to_index.at("STARTED"))=(Events::_FALSE);
}

void agent_router::startAgent()
{
	events.at(events_to_index.at("STOPPED"))=(Events::_FALSE);
	events.at(events_to_index.at("STARTED"))=(Events::_TRUE);
}


bool agent_router::target_reached()
{
	return events.at(events_to_index.at("REACHED"))==Events::_TRUE;
	
}


void agent_router::setSource(lemon::SmartDigraphBase::Node s)
{
	source=s;
}

void agent_router::setTarget(lemon::SmartDigraphBase::Node t)
{
	target=t;
}


// std::pair< int, int > agent_router::getTargetCoords()
// {
// 	return std::pair<int,int> ((coord_x)[next] , (coord_y)[next]);
// }

void agent_router::setGraph(lemon::SmartDigraph& g)
{
	lemon::digraphCopy<lemon::SmartDigraph,lemon::SmartDigraph>(g,graph); //graph=g;
}


bool agent_router::isTimeToNegotiate( simulation_time time )
{
    return round ( time*1000-round ( time/TIME_SLOT_FOR_3DGRAPH ) *1000*TIME_SLOT_FOR_3DGRAPH ) ==-1700;
}


agent_router::~agent_router()
{
}

