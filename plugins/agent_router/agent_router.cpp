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
#include "agent_router_plugin.h"

using namespace std;
using namespace lemon;

graph_packet router_memory_manager::data;
mutex router_memory_manager::mtx;


agent_router::agent_router ( agent* a, Parsed_World* parse ):  length ( graph ),
coord_x ( graph ), coord_y ( graph ),time( a->time ),logger(a->time,a->identifier),
identifier( a->identifier ),communicator(parse->agents.size())//,communicator ( _mutex, &info, _io_service,identifier )
{
    this->a=a;
    this->node_radius=atof(CONFIG.getValue("NODE_RADIUS").c_str());
    this->control_node_radius=node_radius/10.0;
    for (list<Parsed_Agent>::const_iterator iter=parse->agents.begin();iter!=parse->agents.end();++iter)
    {
        if (iter->name==a->identifier)
        targets=(reinterpret_cast<agent_router_parsed_agent*>(iter->parsed_items_from_plugins.at(AGENT_ROUTER_NAME)))->target_list;   //TODO: randomize this in a centralized manager!!
    }
    this->graphName=(reinterpret_cast<agent_router_parsed_world*>(parse->parsed_items_from_plugins.at(AGENT_ROUTER_NAME)))->graphName;

}

bool agent_router::initialize()
{
    speed=&(a->inputs.command.at(a->map_inputs_name_to_id.at("V")));
    omega=&(a->inputs.command.at(a->map_inputs_name_to_id.at("W")));
    x=&(a->state.at(a->map_statename_to_id.at("X")));
    y=&(a->state.at(a->map_statename_to_id.at("Y")));
    theta=&(a->state.at(a->map_statename_to_id.at("THETA")));
    Graph_creator c ( graph, length, coord_x, coord_y );
    graph_node_size = c.createGraph ( MAXFLOORS, graphName );
    if ( !graph_node_size )
    {
        ERR ( "attenzione, impossibile creare il grafo", 0 );
	return false;
    }
    if ( targets.size() < 2 )
    {
      ERR ( "attenzione, la lista dei target e' troppo corta", 0 );
      return false;
    }
    source = graph.nodeFromId ( targets[0] );
    target = graph.nodeFromId ( targets[1] );
    control_target=graph.nodeFromId(targets[1]);
    node_id.push_back ( targets[0] ); //next = source;
    node_id.push_back ( targets[0] ); //next = source;
    target_counter = 2;
    xtarget =  coord_x[source];
    ytarget =  coord_y[source];
    control_xtarget=xtarget;
    control_ytarget=ytarget;
    control_time=getNextTime();
    my_LRP.timestamp = 0;
    started=false;
    stopping=0;
//    setTargetStop(false);
    //communicator.startReceive();
    last_time_updated = time;
    last_time_negotiated = 0;
    last_time_left_a_node=0;
    negotiation_steps=0;
    next_target_reachable=false;
    internal_state=state::STARTING;
    *speed=0;
    priority=identifier;
    initialized=true;
    int best_distance;
    int j=-2;
    dijkstra ( graph, length ).path ( computed_path ).dist ( best_distance ).run ( source, target);
    for ( PathNodeIt<Path<SmartDigraph> > i ( graph, computed_path ); i != INVALID; ++i )
    {
        j++;
        //cout<<graph.id(i)<<endl;
    }
    logger.logBestLength(j);
    //NFO("%s %lf source= %d target= %d",identifier.c_str(),time,targets[0],targets[1]);
    log_buffer.set_capacity(200);
    return true;
}


void agent_router::run_plugin()
{
    //NFO("Position: %lf %s %lf %lf %lf",time,identifier.c_str(),x,y,theta);
    logger.logPosition(*x,*y,*theta);
    if (!initialized)
    return;
    priority=identifier;
    already_received=false;
    if (identifier=="AGENTE2" || identifier=="AGENTE1")
        int do_nothing_important=0;
    if ( internal_state==state::STOPPED )
    {
        prepare_stopped_packet();
        update_packet();
        communicator.send ( my_LRP );
        stopAgent();
        return;
    }
//     if (log_buffer.size()>100)
//         log_buffer.clear();
    collisions_mutex.lock();
    if (collisions[identifier])
    {
        std::ostringstream temp;
        temp<<identifier<<" ";
        for (auto log:log_buffer)
            temp<<log;
        std::cout<<temp.str()<<std::endl;
        log_buffer.clear();
    }
    collisions_mutex.unlock();
    if (internal_state==state::STOPPING) //This is an hack to remove agents from the map
    {
        should_check[identifier]=false;//This is way better (I will keep the hack anyway because of graphical reason)
        if (stopping==0)
        {
            *speed=10000;
            double f = (double)rand() / RAND_MAX;
            *omega=f * (3.0);
        }
        stopping++;
        if (stopping>0)
            internal_state=state::STOPPED;
        return;
    }
    
    auto negotiate=isTimeToNegotiate ( time );

    if ( internal_state==state::MOVING ||  internal_state==state::STARTING ||internal_state==state::LOADING )
    {
        //TODO: check that this isTimeToNegotiate is called only once during the same 10 seconds (maybe use a reset?)
        if ( negotiate ) //negozio anche sugli archi, basta che sia il momento giusto
        {
            internal_state=state::ARC_HANDSHAKING;
            negotiation_steps=0;
//             log_buffer.clear();
        }
        //if ( abs ( getNextTime()- ( time+1.7 ) ) <0.001 )  
        if (negotiate && isNearNode()) //se sono su un nodo invece faccio di piu' che negoziare
        {
            internal_state=state::NODE_HANDSHAKING;
            negotiation_steps=0;
                        
//             log_buffer.clear();
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
                    internal_state=state::STOPPING;
                    return;
                }
            }
        }
    }

    if ( internal_state==state::EMERGENCY )
    {
        if (!already_received)
        {
            info=communicator.receive();
            already_received=true;
        }
        if ( negotiate ) //negozio anche sugli archi, basta che sia il momento giusto
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
        log_buffer.push_back("node handshaking");
        lemon::SmartDigraph::ArcMap<bool> useArc ( graph, true );
        filter_graph ( useArc );
        next_target_reachable=findPath ( useArc );
        if ( !next_target_reachable )
        {
            last_time_left_a_node=-1;
            cout<<"path not found"<<endl;
            log_buffer.push_back("path not found");
            
        }
        else
        {
            last_time_left_a_node=time;
            started=true;
            prepare_move_packet();
            update_packet();
            ostringstream temp;
            temp<<time<<" ";
            print_path(temp);
            log_buffer.push_back(temp.str());
            
            communicator.send ( my_LRP );
        }
    }

    if ( internal_state==state::ARC_HANDSHAKING )
    {
        log_buffer.push_back("arc handshaking");
        lemon::SmartDigraph::ArcMap<bool> useArc ( graph, true );
        filter_graph ( useArc );
        next_target_reachable=findPath ( useArc );
        if ( !next_target_reachable )
        {
            cout<<"path not found"<<endl;
            log_buffer.push_back("path not found");
        }
        else
        {
            prepare_move_packet();
            update_packet();
            ostringstream temp;
            temp<<time<<" ";
            print_path(temp);
            log_buffer.push_back(temp.str());
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

    //The trick of the listening state avoids sooo many packets sent!! Do not expect each agent to send a packet each timestep, it will not!
    if ( internal_state==state::LISTENING || internal_state==state::NODE_HANDSHAKING || internal_state==state::ARC_HANDSHAKING )
    {
//         usleep ( 2000 ); //serve per dare tempo alla comunicazione di girare
//         _io_service.poll();
//         _io_service.reset();
        if (!already_received)
        {
            info=communicator.receive();
            already_received=true;
        }
        negotiation_steps++;
        if ( detect_collision() )
        {
            if (internal_state!=state::NODE_HANDSHAKING && internal_state!=state::ARC_HANDSHAKING) //if I am not in a handshaking, I go back to handshaking
                internal_state=old_state;
            //log_buffer.push_back("new state after detecting collision: "+print_state(internal_state));
            //cout<<time<<" "<<identifier<<" new state after future collision: "<<print_state(internal_state)<<endl;
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
        log_buffer.push_back("emergency state");
    }

    if ( ( internal_state==state::LISTENING || internal_state==state::NODE_HANDSHAKING
            || internal_state==state::ARC_HANDSHAKING ) && negotiation_steps==20 ) //devo aver finito per forza, per ipotesi!
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

//TODO:
//     if ( next_target_reachable )
//     {
//         startAgent();
//     }
//     else
//     {
//         stopAgent();
//     }

    if (next_target_reachable)
    {
        auto next= graph.nodeFromId(node_id[1]);
        xtarget =  coord_x[next];
        ytarget =  coord_y[next];
    }
    if (control_target_reached())
    {
        control_xtarget=xtarget;
        control_ytarget=ytarget;
        control_time=getNextTime();
    }
    setSpeed();
    ostringstream temp;
    temp<<time<<" ";
    print_state(internal_state,temp);
    log_buffer.push_back(temp.str());
//     cout<<" stato interno: ";
//     print_state(internal_state);
//     cout<<endl;
}


//TODO(Mirko): implementare sorpassi multipli
bool agent_router::check_for_overtaking ()
{
    //std::cout<<"check for overtakings";
    bool overtaking = false;
    int myage=findAge ( time,last_time_left_a_node ); //round ( ( round ( time * 1000.0 ) - round ( last_time_left_a_node * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
//     _mutex.lock();
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it )
    {
        if ( identifier==it->first ) continue;
        if (it->second.lockedNode[0]==-1 ) continue;
        if (findAge ( time, it->second.timestamp ) != 0)
        {
            assert ( findAge ( time, it->second.timestamp ) ==0 ); //0==round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH ));
        }
//        assert ( ( *it ).second.lockedNode.size() >1 );
        int age=findAge ( time, it->second.started_moving ); //round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.started_moving * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        for ( unsigned int j = 1; j < ( *it ).second.lockedNode.size(); j++ )
        {
            int other_id = ( *it ).second.lockedNode[j - 1] - ( j==1?age * graph_node_size:0 );
            int other_id1 = ( *it ).second.lockedNode[j];//Attenzione!! - age * graph_node_size;
            if ( other_id1<other_id ) //Impossibile almeno che other_id1 sia al piano terra
            {
                break;
            }
            for ( unsigned int i = 1; i < node_id.size(); i++ )
            {
                int my_id=node_id[i-1]- ( i==1?myage*graph_node_size:0 ); //Attenzione!!
                int my_id1= node_id[i];
                if ( my_id1<my_id || my_id>other_id1 ) //Impossibile almeno che my_id1 sia al piano terra
                {
                    //se my_id>other_id1 inutile controllare i sorpassi
                    break;
                }
                if ( ( my_id - other_id ) % graph_node_size == 0 &&
                        ( my_id1 - other_id1 ) % graph_node_size == 0 )
                {
//                     if ( my_id==other_id )
//                         WARN ( "collisione su un nodo durante il controllo sorpassi? %d %d",my_id,other_id );
//                     if ( my_id1==other_id1 )
//                         WARN ( "collisione su un nodo durante il controllo sorpassi? %d %d",my_id1,other_id1 );
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
//     _mutex.unlock();
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

        //cout<<"controllo se devo entrare in emergenza per colpa dell'agente:"<<it->first<<" "<<endl;
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
                WARN("%s %lf emergenza!!",identifier.c_str(),time);
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
//     _mutex.lock();
    if (there_was_a_collision)
    {
//         std::cout<<time<<" "<<identifier<<"filtering graph after a collision"<<std::endl;
        ostringstream temp;
        temp<<time<<" "<<identifier<<"filtering graph after a collision";
        log_buffer.push_back(temp.str());
        
    }
    for ( graph_packet::const_iterator it = info.begin(); it != info.end(); ++it ) //per ogni pacchetto ricevuto
    {
        if ( identifier==it->first ) continue;
        if (it->second.lockedNode[0]==-1 ) continue;
        if ( priority.compare ( it->second.priority ) <0 ) continue;  //se il pacchetto ha priorita' piu' alta
        int age = findAge ( time, it->second.timestamp ); //round ( ( round ( time * 1000.0 ) - round ( ( *it ).second.timestamp * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );
        //cout<<start<<it->first<<" ";
        start="";
        end="\n";
        for ( vector<int>::const_iterator itt = ( *it ).second.lockedNode.begin(); itt != ( *it ).second.lockedNode.end(); ++itt )  //per ogni nodo nel pacchetto
        {
            int id = ( *itt ) - age * graph_node_size;  //attualizzo il nodo
            if ( id < graph_node_size ) continue; //Se il nodo e' finito nel passato oppure al piano terra lo ignoro
            if (there_was_a_collision)
            {
//                 std::cout<<time<<" "<<identifier<<"nodo filtrato: "<<id%graph_node_size<<std::endl;
                ostringstream temp;
                temp<<time<<" "<<identifier<<"nodo filtrato: "<<id%graph_node_size;
                log_buffer.push_back(temp.str());
            }
            for ( SmartDigraph::InArcIt arc ( graph, graph.nodeFromId ( id ) ); arc != INVALID; ++arc )  //filtro gli archi entranti nel nodo
            {
                useArc[arc] = false;
            }
            //cout<<id<<" ";
        }
        //cout<<"; ";
    }
//     _mutex.unlock();
    //cout<<end;
    //cout.flush();
    there_was_a_collision=false;
}


bool agent_router::detect_collision ( )
{
    bool collision = false;
    std::ostringstream out;
    //out<<"ricerca collisione:";
    int my_age=findAge ( time, last_time_updated ); //round ( ( round ( time * 1000.0 ) - round ( last_time_updated * 1000.0 ) ) / 1000.0 / TIME_SLOT_FOR_3DGRAPH );

//     _mutex.lock();
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
//                     out << time <<" "<<identifier << " rilevata una collisione con "<<it->first<<" sul nodo "<<node_id[i]-my_age*graph_node_size <<"("<<node_id[i]%graph_node_size <<") tra "<<i << " steps;    " <<endl;
                    //NFO("FutureCollision: %lf %s %s %d %d",time, identifier.c_str(),it->first.c_str(),id,id%graph_node_size);
                    logger.logFutureCollision(it->first,id,id%graph_node_size);
                    collision = true;
                    there_was_a_collision=true;
                }
            }
        }
    }
//     _mutex.unlock();
    //out<<"ricerca collisione completata"<<endl;
    if ( collision )
    {
        cout<<out.str();
    }
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

double agent_router::distance_to_target()
{
    return sqrt((xtarget-*x)*(xtarget-*x)+(ytarget-*y)*(ytarget-*y));
}

double agent_router::distance_to_control_target()
{
    return sqrt((control_xtarget-*x)*(control_xtarget-*x)+(control_ytarget-*y)*(control_ytarget-*y)); 
}

void agent_router::setSpeed()
{
    if ( !last_time_left_a_node || last_time_left_a_node==-1||internal_state==state::EMERGENCY )
    {
        *speed=0;
        return;
    }
    assert ( node_id.size() >1 );
    simulation_time delta = control_time-time;//getNextTime() - time;
    double length = distance_to_control_target();
    *speed=length/delta;
    *omega=5*sin(atan2(control_ytarget-*y,control_xtarget-*x)-*theta);
    // std::cout<<identifier<<" speed="<<*speed<<",length="<<length<<"delta="<<delta<<std::endl;
    //NFO("Speed: %lf %s %lf %lf",time, identifier.c_str(),*speed,*omega);
    logger.logSpeed(*speed,*omega);
}

simulation_time agent_router::getNextTime()
{
    if ( !started ) return TIME_SLOT_FOR_3DGRAPH;
    assert ( node_id.size() >1 );
    SmartDigraphBase::Node next=graph.nodeFromId ( node_id[1] );
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

bool agent_router::control_target_reached()
{
    return distance_to_control_target()<control_node_radius;
}

bool agent_router::isNearNode() //Communication distance
{
    return distance_to_target()<node_radius;    
}

void agent_router::setGraph(lemon::SmartDigraph& g)
{
    lemon::digraphCopy<lemon::SmartDigraph,lemon::SmartDigraph>(g,graph); //graph=g;
}


bool agent_router::isTimeToNegotiate( simulation_time time )
{
    if (time-last_time_negotiated<4)
        return false;
    auto temp=time;
    while (temp>TIME_SLOT_FOR_3DGRAPH)
        temp=temp-TIME_SLOT_FOR_3DGRAPH;
    if (temp>7.5)
    {
        last_time_negotiated=time;
        return true;
    }
    return false;
    //return round ( time*1000-round ( time/TIME_SLOT_FOR_3DGRAPH ) *1000*TIME_SLOT_FOR_3DGRAPH ) >=-1700;
}


bool agent_router::setNextTarget()
{
    if ( target_counter == targets.size() )
    {
        //NFO("%s %lf TARGET FINALE raggiunto, mi fermo",identifier.c_str(),time);
        logger.logFinalTarget();
        return false;
    }
    else
    {
        int id = targets[target_counter];
        
        target_counter++;
        target = graph.nodeFromId ( id );

        //NFO("%s %lf TARGET raggiunto, nuovo target: %i %i %i",identifier.c_str(),time,id,coord_x [target],coord_y[target]);
        logger.logNewTarget(id,coord_x[target],coord_y[target]);
        return true;
    }
}
