#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <map>
#include <string>
#include <vector>
#include <list>
#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include <exprtk.hpp>
#include <iostream>
#include <fstream>
//if we are debugging we want a strong typedef, if we are releasing we want the code to be optimized
//we are going to remove all strong_typedef during the code writing
#ifndef NDEBUG
#include <boost/strong_typedef.hpp>
#else
#define BOOST_STRONG_TYPEDEF(a,b) 			\
							typedef a b;
#endif

#define MULTICAST_PORT  30000
#define SINGLECAST_PORT 20000
#define MULTICAST_ADDRESS "239.255.0.1"
#define SOCKET_BINDING "0.0.0.0"
#define MAX_PACKET_LENGTH 1024*32
#define SIMULATOR_PORT 10000
#define SIMULATOR_ROUTE_PORT 10050
#define AGENT_ROUTE_PORT 10051
#define SIMULATOR_GRAPH_PORT 10052
#define AGENT_GRAPH_PORT 10053
#define NUM_AGENTS 1
#define T_CAMP 0.01
#define SYNC_PROTOCOL "tcp://localhost:5761"
							
//written by Alessandro Settimi
#define AGENT_TA_PORT 5580
#define SIMULATOR_TA_PORT 5581
//written by Alessandro Settimi
typedef std::map<int,double> map_int_double;
typedef std::map<int,double> agent_state;
typedef std::map<int,double> control_command;
typedef std::string discrete_state;

/** This map will be used to store informations about variables names and converting them to int
 * In this way we speed up the read access during simulations
 */
typedef std::map<std::string, int> index_map;


enum class Sub_events
{
    _TRUE,
    _FALSE,
    _UNDEFINED
};

enum class Events
{
  _TRUE,
  _FALSE,
  _UNDEFINED
};


/**
 * automaton_state is the representation of a discrete state, an indexMap translates the value of automatonState into a string
 */
typedef int automaton_state;



/**
 * A transition is an event used by the automaton to change his state (automatonState)
 * The map between names of transitions and int is an indexMap
 */
typedef int transition;


/** The time used by the simulator and sent to agents for synchronization */
typedef double simulation_time;

template< typename K,typename T>//,typename C, typename Alloc >
std::ostream& operator<<( std::ostream& os, const std::map<K,T>& m )
{
    for (typename std::map<K,T>::const_iterator it=m.begin();it!=m.end();++it)
        os<<it->first <<" "<<it->second <<" ";
    os<<std::endl;
    return os;
}

template< typename T>//,typename C, typename Alloc >
std::ostream& operator<<( std::ostream& os, const std::vector<T>& m )
{
    for (typename std::vector<T>::const_iterator it=m.begin();it!=m.end();++it)
        os<<(*it)<<" ";
    return os;
}

struct empty_packet
{
	std::string agent_id;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& agent_id;
	}
};


struct graph_informations
{
    bool isNegotiating;
    std::vector<int> lockedNode;
    std::vector<int> lockedArc;
    unsigned int priority; //priorita' durante l'handshake
    std::string id; //id dell'agente
    simulation_time timestamp; //Un giorno ci metteremo il tempo reale (dal 1970)
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& timestamp;
        ar& isNegotiating;
        ar& lockedNode;
        ar& lockedArc;
        ar& id;
    }
    friend std::ostream& operator<< (std::ostream& os, const graph_informations& g)
    {
        os<<g.timestamp<<" "<<g.lockedNode<<" "<<g.lockedArc<<" "<<g.id;
        return os;
    }

};

typedef std::map<std::string,graph_informations> graph_packet;



struct agent_state_packet
{
    std::map<int,double> state;
    std::string identifier;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& state;
        ar& identifier;
    }
};

struct agents_name_to_states
//TODO(MIRKO): Controllare se questa struttura serve oppure può essere inclusa in World_sim_packet
{
  friend class boost::serialization::access;
  std::map<std::string,agent_state_packet> internal_map;

private:
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& internal_map;

    }
};


struct world_sim_packet {
    std::map<std::string,double> bonus_variables;
    agents_name_to_states state_agents;
	simulation_time time;
    template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
		ar& time;
        ar& bonus_variables;
        ar& state_agents;
    }

};

struct control_command_packet
{
    std::map<automaton_state ,control_command> commands;
    control_command default_command;
    std::string identifier;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
	ar& default_command;
        ar& identifier;
    }
};


struct visibleArea
{
    virtual bool contains(agent_state me,agent_state other);
    virtual ~visibleArea();
};

struct circle:public visibleArea
{
    double radius;
    int first_axis;
    int second_axis;

    bool contains(agent_state me,agent_state other)
    {
        return pow(me.at(first_axis)-other.at(first_axis),2)+pow(me.at(second_axis)-other.at(second_axis),2)<pow(radius,2);
    }
};

//written by Alessandro Settimi
namespace task_assignment_namespace
{
typedef double task_cost;
typedef std::string task_id;
typedef std::string agent_id;
typedef std::map<task_id,task_cost> task_cost_vector;
typedef std::map<task_assignment_namespace::agent_id,task_assignment_namespace::task_cost_vector> task_cost_matrix;
typedef std::map<task_id,bool> task_assignment_vector;
typedef std::map<task_assignment_namespace::agent_id,task_assignment_namespace::task_assignment_vector> task_assignment_matrix;
typedef int task_assignment_algorithm;

struct task
{
    std::string task_id;
    double task_position[3];
    int task_type;
    double task_execution_time;
    double task_deadline;
};
typedef std::vector<task> task_list;


template <typename data_type>
class task_assignment_packet
{
public:
    
    std::string agent_id;
    data_type data;
    
    task_assignment_packet()
    {}
    
    void set_data(void *data)
    {
	  this->data=*(data_type*)data;
    }
    
    void* get_data()
    {
	  return &data;
    }
    
    template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
	ar& agent_id;
        ar& data;
    }
};

struct solution_cost_packet
{
      task_assignment_matrix ta_matrix;
      std::map<task_id,task_cost> costs;
      
      template <typename Archive>
      void serialize(Archive& ar,const unsigned int /*version*/)
      {
	ar& ta_matrix;
        ar& costs;
      }
};

typedef task_assignment_packet<double> subgradient_packet;
typedef task_assignment_packet<std::map<agent_id,task_assignment_vector>> solution_exchange_packet;
typedef task_assignment_packet<solution_cost_packet> cost_exchange_packet;

}
#ifndef GLP_MIN
#define GLP_MIN 1
#endif

#ifndef GLP_MAX
#define GLP_MAX 2
#endif

#define INF 1000.0

#define SUBGRADIENT 0
#define SOLUTION_EXCHANGE 1
#define COST_EXCHANGE 2
//written by Alessandro Settimi

#endif //TYPEDEFS_H
