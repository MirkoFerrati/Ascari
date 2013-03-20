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
#include <mutex>
#include<lemon/graph_to_eps.h>
#include "debug_constants.h"
#include "graph_creator.h"




using namespace std;
using namespace lemon;

agent_router::agent_router ( std::vector< int > tarlist, std::map< transition, Events >& events,
                             const std::map<std::string, transition>& events_to_index, string identifier,
                             simulation_time& time, std::string graphName
                           )
    : targets ( tarlist ), events ( events ), events_to_index ( events_to_index ), length ( graph ),
      coord_x ( graph ), coord_y ( graph ), identifier ( identifier ),
      communicator ( _mutex, &info, _io_service,identifier ), time ( time )
{
    Graph_creator c ( graph, length, coord_x, coord_y );
    graph_node_size = c.createGraph ( MAXFLOORS, graphName );
    if ( !graph_node_size )
        ERR ( "attenzione, impossibile creare il grafo", 0 );
    if ( targets.size() < 2 )
        ERR ( "attenzione, la lista dei target e' troppo corta", 0 );
    source = graph.nodeFromId ( targets[0] );
    target = graph.nodeFromId ( targets[1] );
    target_counter = 2;
    next = source;
    xtarget = coord_x[next];
    ytarget = coord_y[next];
    my_LRP.timestamp = 0;
//    setTargetStop(false);
    communicator.startReceive();
    last_time_updated = time;
    negotiation_steps=0;
    internal_state=state::STOPPED;
    speed=0;
    priority=identifier;
}

void agent_router::run_plugin()
{
    if ( internal_state==state::STOPPED )
    {
        stopAgent();
        return;
    }
    if ( target_reached() )
    {
        setNextTarget();
    }

    if ( internal_state==state::MOVING || internal_state==state::EMERGENCY)
    {
        if ( isTimeToNegotiate() )
        {
            internal_state=state::HANDSHAKING;
	    negotiation_steps=0;
        }
        else return;
    }

    if ( internal_state==state::HANDSHAKING )
    {
        lemon::SmartDigraph::ArcMap<bool> useArc ( graph, true );

        filter_graph ( useArc );

        next_target_reachable=findPath ( useArc );
        if ( next_target_reachable )
        {
            prepare_move_packet();
            update_packet();
            communicator.send ( my_LRP );
        }
        
    }
    
    if (internal_state==state::LISTENING || internal_state==state::HANDSHAKING)
    {
	usleep(200000); //TODO: serve per dare tempo alla comunicazione di girare
	_io_service.poll();
	_io_service.reset();
	negotiation_steps++;
    }
    
    if (!detect_collision())
    {
      internal_state=state::LISTENING;
    }
    else
    {
     internal_state=state::HANDSHAKING; 
    }

    if (negotiation_steps==15)
    {
      if (next_target_reachable)
      {
	  check_for_overtaking(); 
	  internal_state=state::MOVING;
      }
      else
     {
       internal_state=state::EMERGENCY;
       prepare_emergency_packet();
       update_packet();
       communicator.send(my_LRP);
     }
    }
    
    
    if ( next_target_reachable )
    {
        startAgent();
    }
    else
    {
        stopAgent();
    }
    
    setSpeed();
}


//TODO(Mirko): implementare sorpassi multipli
bool agent_router::check_for_overtaking ()
{
    std::cout<<"check for overtakings";
    bool overtaking = false;
    _mutex.lock();
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it )
    {
        int age = round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        for ( unsigned int j = 1; j < ( *it ).second.lockedNode.size(); j++ )
        {
            int id = ( *it ).second.lockedNode[j - 1] - age * graph_node_size;
            int id1 = ( *it ).second.lockedNode[j] - age * graph_node_size;

            for ( unsigned int i = 1; i < node_id.size(); i++ )
            {
                if ( ( node_id[i - 1] - id ) % graph_node_size == 0 &&
                        ( node_id[i] - id1 ) % graph_node_size == 0 )
                {
                    if (
                        node_id[i - 1] > id &&
                        node_id[i] < id1 )
                    {
                        overtaking = true;
                        std::cout << time << ": sto rischiando di sorpassare l'agente " << ( *it ).second.id << " tra il nodo " << id << " e " << id1 << "passando da " << node_id[i - 1] << " e " << node_id[i] << "\n"; //<<std::endl;
                        node_id[i]=id1;
                    }
                    if ( node_id[i - 1] < id &&
                            node_id[i] > id1 )
                    {
                        overtaking = true;
                        std::cout << time << ": sto rischiando di essere sorpassato dall'agente " << ( *it ).second.id << " tra il nodo " << id << " e " << id1 << "passando da " << node_id[i - 1] << " e " << node_id[i] << "\n"; //<<std::endl;
                        node_id[i]=id1;
                    }
                }
            }
        }
    }
    _mutex.unlock();
    return overtaking;
}


void agent_router::filter_graph ( lemon::DigraphExtender< lemon::SmartDigraphBase >::ArcMap< bool >& useArc )
{
    _mutex.lock();
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it ) //per ogni pacchetto ricevuto
    {
        if ( identifier.compare ( it->first ) <0 ) continue;  //se il pacchetto ha priorita' piu' alta
        int age = round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        cout<<"filtro i nodi:"<<endl;
        for ( vector<int>::const_iterator itt = ( *it ).second.lockedNode.begin(); itt != ( *it ).second.lockedNode.end(); ++itt )  //per ogni nodo nel pacchetto
        {
            int id = ( *itt ) - age * graph_node_size;  //attualizzo il nodo
            if ( id < graph_node_size ) continue; //Se il nodo e' finito nel passato oppure al piano terra lo ignoro
            for ( SmartDigraph::InArcIt arc ( graph, graph.nodeFromId ( id ) ); arc != INVALID; ++arc )  //filtro gli archi entranti nel nodo
            {
                useArc[arc] = false;
            }
        }
    }
    _mutex.unlock();
}


bool agent_router::detect_collision ( )
{
    bool collision = false;
    _mutex.lock();
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it )
    {
        if ( identifier.compare ( it->first ) <0 ) continue;
        int age = round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        int my_age=round ( ( round ( time * 1000.0 ) - round ( last_time_updated * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        cout<<"filtro i nodi:"<<endl;
        for ( vector<int>::const_iterator itt = ( *it ).second.lockedNode.begin(); itt != ( *it ).second.lockedNode.end(); ++itt )
        {
            int id = ( *itt ) - age * graph_node_size;
            if ( id < graph_node_size ) //Se il nodo e' finito nel passato oppure al piano terra lo ignoro
                continue;
            //Calcolo le collisioni
            for ( unsigned int i = 0; i < node_id.size(); i++ )
            {
                if ( node_id[i]-my_age*graph_node_size == id )
                {
                    {
                        cout << time << " rilevata una collisione con "<<it->first<<" tra "<<node_id[i]-my_age*graph_node_size <<"("<<i <<") e "<<id << endl;
                    }
                    collision = true;
                }
            }
            cout<<" "<<id;
        }
        cout<<endl;
    }
    _mutex.unlock();
    return collision;
}


bool agent_router::findPath ( lemon::SmartDigraph::ArcMap<bool>& useArc )
{
    assert ( target != INVALID );
    int distance;
    bool reached = dijkstra ( filterArcs ( graph, useArc ), length ).path ( computed_path ).dist ( distance ).run ( source, target );
    if ( distance > MAX_LENGTH )
        reached = false;
    if ( !reached )
    {
        next = INVALID;
        next_time=-1;
        return false;
    }
    next = graph.target ( computed_path.front() );
    xtarget =  coord_x[next];
    ytarget =  coord_y[next];
    unsigned int floor = graph.id ( next ) / graph_node_size;
    // next_time = trunc ( trunc ( time + TIME_SLOT_FOR_3DGRAPH / 3.0 ) / TIME_SLOT_FOR_3DGRAPH ) * TIME_SLOT_FOR_3DGRAPH + TIME_SLOT_FOR_3DGRAPH * floor; //TODO bruttissimo
    next_time = round ( time/TIME_SLOT_FOR_3DGRAPH ) * TIME_SLOT_FOR_3DGRAPH + TIME_SLOT_FOR_3DGRAPH * floor;
    return true;
}


void agent_router::update_packet()
{
    my_LRP.id = identifier;
    my_LRP.priority = priority;
    my_LRP.lockedNode = node_id;
    my_LRP.timestamp = last_time_updated;
    my_LRP.emergency=internal_state==state::EMERGENCY;
}

void agent_router::prepare_emergency_packet()
{
    node_id.clear();
    for ( unsigned int i = 0; i < MAXFLOORS; i++ ) //prenoto qualche nodo sopra di me per sicurezza
    {
        node_id.push_back ( graph.id ( source ) % graph_node_size + i * graph_node_size );
    }
    last_time_updated = time;
}

void agent_router::prepare_move_packet()
{
    using namespace lemon;
    node_id.clear();
    for ( PathNodeIt<Path<SmartDigraph> > i ( graph, computed_path ); i != INVALID; ++i )
    {
        //Il nodo al piano zero (quello iniziale ) e quello al piano finale non vanno inclusi
        if ( graph.id ( i ) / graph_node_size > FLOORS_SENT || graph.id ( i ) < graph_node_size )
            continue;
        node_id.push_back ( graph.id ( i ) );
    }
    last_time_updated = time;
}


void agent_router::setNextTarget()
{
    source = graph.nodeFromId ( graph.id ( next ) % graph_node_size );
    if ( graph.id ( next ) % graph_node_size == graph.id ( target ) )
    {
        if ( target_counter == targets.size() )
        {
            cout << " TARGET FINALE raggiunto, mi fermo" << endl;
            internal_state = state::STOPPED;
        }
        else
        {
            int id = targets[target_counter];
            target_counter++;
            target = graph.nodeFromId ( id );
            cout << time << ": TARGET raggiunto, nuovo target: " << id << " " << coord_x [target] << " " << coord_y[target] << endl;
        }
    }
}
