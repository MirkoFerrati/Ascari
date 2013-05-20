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
#include <mutex>
#include <lemon/graph_to_eps.h>
#include "debug_constants.h"
#include "graph_creator.h"
#include "agent_router_parsed_agent.h"
#include "agent_router_parsed_world.h"




using namespace std;
using namespace lemon;

agent_router::agent_router ( std::vector< int > tarlist, std::map< transition, Events >& events,
                             const std::map<std::string, transition>& events_to_index, string identifier,
                             simulation_time& time, std::string graphName
                           )
    :  events ( events ), events_to_index ( events_to_index ), length ( graph ),
       coord_x ( graph ), coord_y ( graph ),
       targets ( tarlist ),time ( time ),identifier ( identifier ),communicator ( _mutex, &info, _io_service,identifier )
{
 initialize(graphName);
}

agent_router::agent_router ( agent* a, Parsed_World* parse ):  events ( a->events ), events_to_index ( a->events_to_index ), length ( graph ),
       coord_x ( graph ), coord_y ( graph ),
       /*targets ( tarlist ),*/time ( a->time ),identifier ( a->identifier ),communicator ( _mutex, &info, _io_service,identifier )
{
  targets=(reinterpret_cast<agent_router_parsed_agent*>(parse->agents.front().parsed_items_from_plugins[0]))->target_list; //funziona perche' rimane un solo agente nel mondo(gli altri vengono eliminati dal main)
  std::string graphName=(reinterpret_cast<agent_router_parsed_world*>(parse->parsed_items_from_plugins[0]))->graphName;
initialize(graphName);
}

bool agent_router::initialize(std::string graphName)
{
   Graph_creator c ( graph, length, coord_x, coord_y );
    graph_node_size = c.createGraph ( MAXFLOORS, graphName );
    if ( !graph_node_size )
        ERR ( "attenzione, impossibile creare il grafo", 0 );
    if ( targets.size() < 2 )
        ERR ( "attenzione, la lista dei target e' troppo corta", 0 );
    source = graph.nodeFromId ( targets[0] );
    target = graph.nodeFromId ( targets[1] );
    node_id.push_back ( targets[0] ); //next = source;
    node_id.push_back ( targets[0] ); //next = source;
    target_counter = 2;
    xtarget = 0;//coord_x[next];
    ytarget = 0;//coord_y[next];
    my_LRP.timestamp = 0;
    started=false;
//    setTargetStop(false);
    communicator.startReceive();
    last_time_updated = time;
    last_time_left_a_node=0;
    negotiation_steps=0;
    next_target_reachable=false;
    internal_state=state::STARTING;
    speed=0;
    priority=identifier;
    return true;
}


void agent_router::run_plugin()
{
    priority=identifier;

    if ( internal_state==state::STOPPED )
    {
        prepare_stopped_packet();
        update_packet();
        communicator.send ( my_LRP );
        stopAgent();
        return;
    }

    if ( internal_state==state::MOVING ||  internal_state==state::STARTING ||internal_state==state::LOADING )
    {
        if ( isTimeToNegotiate ( time ) ) //negozio anche sugli archi, basta che sia il momento giusto
        {
            internal_state=state::ARC_HANDSHAKING;
            negotiation_steps=0;
        }
        if ( abs ( getNextTime()- ( time+1.7 ) ) <0.001 )  //se sono su un nodo invece faccio di piu' che negoziare
        {
            internal_state=state::NODE_HANDSHAKING;
            negotiation_steps=0;
            if ( isOnTarget() ) //se il nodo e' il mio target
            {
                auto oldtarget=graph.id ( target );
                if ( setNextTarget() )  //se ci sono altri target
                {
                    if ( oldtarget==graph.id ( target ) ) //se il target e' ripetuto vuol dire che devo fare loading
                    {
                        internal_state=state::LOADING;
                        negotiation_steps=0;

                    }
                }
                else  //se non ci sono altri target
                {
                    internal_state=state::STOPPED;
                    return;
                }
            }
        }
    }

    if ( internal_state==state::EMERGENCY )
    {
        usleep ( 2000 ); //TODO: serve per dare tempo alla comunicazione di girare
        _io_service.poll();
        _io_service.reset();
        if ( isTimeToNegotiate ( time ) ) //negozio anche sugli archi, basta che sia il momento giusto
        {
            internal_state=state::NODE_HANDSHAKING;
            negotiation_steps=0;
        }
        prepare_emergency_packet();
        update_packet();
        communicator.send ( my_LRP );
    }

    if ( internal_state==state::NODE_HANDSHAKING )
    {
        lemon::SmartDigraph::ArcMap<bool> useArc ( graph, true );
        filter_graph ( useArc );
        next_target_reachable=findPath ( useArc );
        if ( !next_target_reachable )
        {
            last_time_left_a_node=-1;
            cout<<"path not found"<<endl;
        }
        else
        {
            last_time_left_a_node=time;
            prepare_move_packet();
            update_packet();
            print_path();
            communicator.send ( my_LRP );
        }
    }

    if ( internal_state==state::ARC_HANDSHAKING )
    {
        lemon::SmartDigraph::ArcMap<bool> useArc ( graph, true );
        filter_graph ( useArc );
        next_target_reachable=findPath ( useArc );
        if ( !next_target_reachable )
        {
            cout<<"path not found"<<endl;
        }
        else
        {
            prepare_move_packet();
            update_packet();
            print_path();
            communicator.send ( my_LRP );
        }
    }

    if ( internal_state==state::LOADING )
    {
        prepare_loading_packet();
        priority=" "; //BRUTTO HACK
        update_packet();
        communicator.send ( my_LRP );
        negotiation_steps++;
        if ( negotiation_steps==15 )
            internal_state=state::MOVING;
    }

    if ( internal_state==state::LISTENING || internal_state==state::NODE_HANDSHAKING || internal_state==state::ARC_HANDSHAKING )
    {
        usleep ( 2000 ); //TODO: serve per dare tempo alla comunicazione di girare
        _io_service.poll();
        _io_service.reset();
        negotiation_steps++;
        if ( detect_collision() )
        {
            internal_state=old_state;
        }
        else
        {
            if ( internal_state!=state::LISTENING )
            {
                old_state=internal_state;
            }
            internal_state=state::LISTENING;
        }

    }

    if ( isEmergency ( node_id ) )
    {
        internal_state=state::EMERGENCY;
        prepare_emergency_packet();
        update_packet();
        communicator.send ( my_LRP );
    }

    if ( ( internal_state==state::LISTENING || internal_state==state::NODE_HANDSHAKING
            || internal_state==state::ARC_HANDSHAKING ) && negotiation_steps==15 ) //devo aver finito per forza, per ipotesi!
    {
        if ( next_target_reachable )
        {
            check_for_overtaking();
            internal_state=state::MOVING;
        }
        else
        {
            internal_state=state::EMERGENCY;
            prepare_emergency_packet();
            update_packet();
            communicator.send ( my_LRP );
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
    //cout<<"stato interno: ";
    //print_state(internal_state);
    //cout<<endl;
}


//TODO(Mirko): implementare sorpassi multipli
bool agent_router::check_for_overtaking ()
{
    //std::cout<<"check for overtakings";
    bool overtaking = false;
    int myage=findAge ( time,last_time_left_a_node ); //round ( ( round ( time * 1000.0 ) - round ( last_time_left_a_node * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
    _mutex.lock();
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it )
    {
        if ( identifier==it->first ) continue;
        assert ( findAge ( time, it->second.timestamp ) ==0 ); //0==round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH ));
//        assert ( ( *it ).second.lockedNode.size() >1 );
        int age=findAge ( time, it->second.started_moving ); //round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.started_moving * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        for ( unsigned int j = 1; j < ( *it ).second.lockedNode.size(); j++ )
        {
            int other_id = ( *it ).second.lockedNode[j - 1] - ( j==1?age * graph_node_size:0 );
            int other_id1 = ( *it ).second.lockedNode[j];//TODO: Attenzione!! - age * graph_node_size;
            if ( other_id1<other_id ) //Impossibile almeno che other_id1 sia al piano terra
            {
                break;
            }
            for ( unsigned int i = 1; i < node_id.size(); i++ )
            {
                int my_id=node_id[i-1]- ( i==1?myage*graph_node_size:0 ); //TODO: Attenzione!!
                int my_id1= node_id[i];
                if ( my_id1<my_id || my_id>other_id1 ) //Impossibile almeno che my_id1 sia al piano terra
                {
                    //se my_id>other_id1 inutile controllare i sorpassi
                    break;
                }
                if ( ( my_id - other_id ) % graph_node_size == 0 &&
                        ( my_id1 - other_id1 ) % graph_node_size == 0 )
                {
                    if ( my_id==other_id )
                        WARN ( "collisione su un nodo durante il controllo sorpassi? %d %d",my_id,other_id );
                    if ( my_id1==other_id1 )
                        WARN ( "collisione su un nodo durante il controllo sorpassi? %d %d",my_id1,other_id1 );
                    if ( my_id > other_id && my_id1< other_id1 )
                    {
                        overtaking = true;
                        std::cout << time << ": sto rischiando di sorpassare l'agente " << ( *it ).second.id << " tra il nodo " << other_id << " e " << other_id1 << "passando da " << my_id << " e " << my_id1 << "\n"; //<<std::endl;
                        node_id[i]=other_id1;
                    }
                    if ( my_id < other_id && my_id1 > other_id1 )
                    {
                        overtaking = true;
                        std::cout << time << ": sto rischiando di essere sorpassato dall'agente " << ( *it ).second.id << " tra il nodo " << other_id << " e " << other_id1 << "passando da " << my_id << " e " << my_id1 << "\n"; //<<std::endl;
                        node_id[i]=other_id1;
                    }
                }
            }
        }
    }
    _mutex.unlock();
    cout.flush();
    return overtaking;
}

bool agent_router::isEmergency ( const std::vector<int>& nodes )
{
    std::unique_lock<std::mutex> ( _mutex ); //.lock(); invece di lockare dato che ho piu return uso la unique_lock
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it ) //per ogni pacchetto ricevuto
    {
        if ( identifier==it->first ) continue;
        if ( !it->second.emergency && !it->second.priority.compare ( " " ) ==0 ) continue;

        cout<<"controllo se devo entrare in emergenza per colpa dell'agente:"<<it->first<<" "<<endl;
        for ( vector<int>::const_iterator itt = ( *it ).second.lockedNode.begin(); itt != ( *it ).second.lockedNode.end(); ++itt )  //per ogni nodo nel pacchetto
        {
            int id = ( *itt );// - age * graph_node_size;  //attualizzo il nodo
            //     for ( auto node:nodes )
            auto node=nodes[1];
            //         {
            //              i++;
//                if ( node/graph_node_size ) break;
            if ( node%graph_node_size==id%graph_node_size &&  getNextTime()-time<2*TIME_SLOT_FOR_3DGRAPH )
            {
                cout<<"emergenza!!"<<endl;
                return true;
            }

//             }
        }

    }
    //cout<<"nessuna emergenza"<<endl;
    return false;
}


void agent_router::filter_graph ( lemon::DigraphExtender< lemon::SmartDigraphBase >::ArcMap< bool >& useArc )
{
    string start="filtro i nodi:";
    string end="";
    _mutex.lock();

    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it ) //per ogni pacchetto ricevuto
    {
        if ( identifier==it->first ) continue;
        if ( priority.compare ( it->second.priority ) <0 ) continue;  //se il pacchetto ha priorita' piu' alta
        int age = findAge ( time, it->second.timestamp ); //round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        cout<<start<<it->first<<" ";
        start="";
        end="\n";
        for ( vector<int>::const_iterator itt = ( *it ).second.lockedNode.begin(); itt != ( *it ).second.lockedNode.end(); ++itt )  //per ogni nodo nel pacchetto
        {
            int id = ( *itt ) - age * graph_node_size;  //attualizzo il nodo
            if ( id < graph_node_size ) continue; //Se il nodo e' finito nel passato oppure al piano terra lo ignoro
            for ( SmartDigraph::InArcIt arc ( graph, graph.nodeFromId ( id ) ); arc != INVALID; ++arc )  //filtro gli archi entranti nel nodo
            {
                useArc[arc] = false;
            }
            cout<<id<<" ";
        }
        cout<<"; ";
    }
    _mutex.unlock();
    cout<<end;
    cout.flush();
}


bool agent_router::detect_collision ( )
{
    bool collision = false;
    std::ostringstream out;
    out<<"ricerca collisione:";
    int my_age=findAge ( time, last_time_updated ); //round ( ( round ( time * 1000.0 ) - round ( last_time_updated * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );

    _mutex.lock();
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it )
    {
        if ( identifier==it->first ) continue;
        if ( priority.compare ( it->second.priority ) <0 ) continue;
        int age = findAge ( time, it->second.timestamp ); //round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        for ( vector<int>::const_iterator itt = ( *it ).second.lockedNode.begin(); itt != ( *it ).second.lockedNode.end(); ++itt )
        {
            int id = ( *itt ) - age * graph_node_size;
            if ( id < graph_node_size ) //Se il nodo e' finito nel passato oppure al piano terra lo ignoro
            {
                continue;
            }
            //Calcolo le collisioni
            for ( unsigned int i = 0; i < node_id.size(); i++ )
            {
                if ( node_id[i]-my_age*graph_node_size == id )
                {
                    {
                        out << time << " rilevata una collisione con "<<it->first<<" tra "<<node_id[i]-my_age*graph_node_size <<"("<<i <<") e "<<id << ";    ";
                    }
                    collision = true;
                }
            }
        }
    }
    _mutex.unlock();
    out<<"ricerca collisione completata"<<endl;
    if ( collision )
        cout<<out.str();
    return collision;
}


bool agent_router::findPath ( lemon::SmartDigraph::ArcMap<bool>& useArc )
{
    assert ( target != INVALID );
//     int best_distance;
//     dijkstra ( graph, length ).path ( computed_path ).dist ( best_distance ).run ( source, target );
    int distance;
    bool reached = dijkstra ( filterArcs ( graph, useArc ), length ).path ( computed_path ).dist ( distance ).run ( source, target );
    if ( distance > MAX_LENGTH )
        reached = false;
//     if (distance>3*best_distance)
//       reached=false;
    return reached;

}


void agent_router::update_packet()
{
    my_LRP.id = identifier;
    my_LRP.priority = priority;
    my_LRP.lockedNode = node_id;
    my_LRP.timestamp = last_time_updated;
    my_LRP.started_moving = last_time_left_a_node;
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
        //Il nodo al piano finale non va incluso e posso uscire
        if ( graph.id ( i ) / graph_node_size >= FLOORS_SENT ) // || graph.id ( i ) < graph_node_size )
            break;
        node_id.push_back ( graph.id ( i ) );
    }
    last_time_updated = time;
}
void agent_router::prepare_stopped_packet()
{
    using namespace lemon;
    node_id.clear();
    node_id.push_back ( -1 );
    node_id.push_back ( -1 );
    last_time_updated = time;
}

void agent_router::prepare_loading_packet()
{
    using namespace lemon;

    node_id.clear();
    int i = 0;//target_counter;
    node_id.push_back ( graph.id ( source ) );
    assert ( targets[target_counter-1]==graph.id ( target ) );
    while ( targets[target_counter-1+i]==graph.id ( target ) )
    {
        node_id.push_back ( graph.id ( target ) +graph_node_size* ( i+1 ) );
        i++;
    }
    last_time_updated = time;
}


void agent_router::setSpeed()
{
    if ( !last_time_left_a_node || last_time_left_a_node==-1||internal_state==state::EMERGENCY )
    {
        speed=0;
        return;
    }
    started=true;
    assert ( node_id.size() >1 );
    simulation_time delta = getNextTime() - time;
    double length = distance_to_target.value();
    speed=length/delta;
    //cout<<"speed="<<speed<<",length="<<length<<"delta="<<delta<<endl;
}

simulation_time agent_router::getNextTime()
{
    if ( !started ) return TIME_SLOT_FOR_3DGRAPH;
    assert ( node_id.size() >1 );
    SmartDigraphBase::Node next=graph.nodeFromId ( node_id[1] );
    xtarget =  coord_x[next];
    ytarget =  coord_y[next];
    unsigned int floor = graph.id ( next ) / graph_node_size;
    int next_time = round ( last_time_updated/TIME_SLOT_FOR_3DGRAPH ) *TIME_SLOT_FOR_3DGRAPH + TIME_SLOT_FOR_3DGRAPH * floor;
    return next_time;
}

bool agent_router::isOnTarget()
{
    assert ( node_id.size() >1 );
    source = graph.nodeFromId ( node_id[1] % graph_node_size );
    if ( graph.id ( source ) == graph.id ( target ) )
        return true;
    return false;
}

bool agent_router::setNextTarget()
{
    if ( target_counter == targets.size() )
    {
        cout << " TARGET FINALE raggiunto, mi fermo" << endl;
        return false;
    }
    else
    {
        int id = targets[target_counter];
        target_counter++;
        target = graph.nodeFromId ( id );
        cout << time << ": TARGET raggiunto, nuovo target: " << id << " " << coord_x [target] << " " << coord_y[target] << endl;
        return true;
    }
}
