#include "agent_router.h"
#include <lemon/dijkstra.h>
#include <lemon/path.h>
#include <lemon/random.h>
#include <time.h>
#include <agent.h>
#include <lemon/adaptors.h>
#include <cassert>
#include "typedefs.h"
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


void agent_router::setTargetStop(bool stop)
{
	events.at(events_to_index.at("STOPPED"))=(stop?Events::_TRUE:Events::_FALSE);
	events.at(events_to_index.at("STARTED"))=(!stop?Events::_TRUE:Events::_FALSE);
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


std::pair< int, int > agent_router::getTargetCoords()
{
	return std::pair<int,int> ((coord_x)[next] , (coord_y)[next]);
}

void agent_router::setGraph(lemon::SmartDigraph& g)
{
	lemon::digraphCopy<lemon::SmartDigraph,lemon::SmartDigraph>(g,graph); //graph=g;
}

ostream& agent_router::toFile(ostream& out) const
{
	using namespace lemon;
	for (PathNodeIt<Path<SmartDigraph> > i(graph,p); i != INVALID; ++i)
		out<<" "<<(coord_x)[i]<<" "<<(coord_y)[i];
	if (target!=INVALID)
		out<<" "<<(coord_x)[target]<<" "<<(coord_y)[target];
	out<<endl;
	return out;
}

agent_router::~agent_router()
{
}

