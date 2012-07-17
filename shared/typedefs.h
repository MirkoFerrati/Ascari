#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <map>
#include <string>
#include <list>
#include "boost/serialization/map.hpp" 
#include <exprtk.hpp>

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
#define MAX_PACKET_LENGTH 1024
#define SIMULATOR_PORT 10000
#define SIMULATOR_ROUTE_PORT 10050
#define AGENT_ROUTE_PORT 10051
#define NUM_AGENTS 1
#define T_CAMP 0.01

//Used in the strong_typedef
typedef std::map<int,double> map_int_double;


/**
 * Represents a state of an agent, where the key of the map is the index of the variable as indicated from an indexMap
 */
// struct agent_state: map_int_double
// {
// 	template <typename Archive>
// 	void serialize(Archive& ar, const unsigned int version)
// 	{
// 		ar& *this;
// 	}
// };

typedef std::map<int,double> agent_state;
typedef std::map<int,double> control_command;
/**
 * Represents a control command, where the key of the map is the index of the variable as indicated from an indexMap
 */
// struct control_command :map_int_double
// {
// 	
// 	template <typename Archive>
// 	void serialize(Archive& ar, const unsigned int version)
// 	{
// 		ar& *this;
// // 		for (std::map<int,double>::const_iterator it= control_map.begin();it!=control_map.end();it++)
// // 		{
// // 			ar& it->first;
// // 			ar& it->second;
// // 		}
// 			
// 	}
// };


/** This map will be used to store informations about variables names and converting them to int
 * In this way we speed up the read access during simulations
 */
typedef std::map<std::string, int> index_map;

enum sub_event_value
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
BOOST_STRONG_TYPEDEF( int, transition)


/** The time used by the simulator and sent to agents for synchronization */
typedef double simulation_time;

struct graph_informations
{
	bool isLocked;
	int lockedNode;
	int lockedArc;
	std::string id;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& isLocked;
		ar& lockedNode;
		ar& lockedArc;
		ar& id;
	}
	
};



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
{
	std::map<std::string,agent_state_packet> internal_map;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& internal_map;
		
	}
};


struct world_sim_packet{
  std::map<std::string,double> bonus_variables;
  agents_name_to_states state_agents;

  template <typename Archive>
  void serialize(Archive& ar,const unsigned int /*version*/)
  {
	ar& bonus_variables;
	ar& state_agents;
  }
  
};

struct control_command_packet
{
	control_command command;
	std::string identifier;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& command;
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



template <typename T>
struct rndom : public exprtk::ifunction<T> {
        rndom() : exprtk::ifunction<T>(2) {name="RNDOM";	}

        std::string name;
        
        // Returns random number. v1 is inclusive and v2 is inclusive too.
        T operator()(const T& v1, const T& v2) {

            // If v1 or v2 are smaller than 0 or v2 is smaller than v1 (v1 is min, v2 is max)
            // or v2 is bigger than RAND_MAX, then return nan.
            if (v2 < v1 || v2 > RAND_MAX) {
                return std::numeric_limits<T>::quiet_NaN();
            }

            double min = v1;
            double max = v2;

            if (max < min) {
                return T(min);
            }

            double result;
            result = (rand()*(max-min)/RAND_MAX+min);
			//% (max + 1 - min)) + min;
            return T(result);
        }
};


#endif //TYPEDEFS_H