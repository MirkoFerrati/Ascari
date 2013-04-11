#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include "types.h"
#include "streams_utils.h"

class task_assignment_task;

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

struct empty_packet
{
	std::string agent_id;
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& agent_id;
	}
};


struct dummy_state
{
  int automatonState;
  agent_state state;
  bool operator ==(const dummy_state & other) const
  {
    if ((*this).automatonState==other.automatonState)
      return true;
      else
	return false;
  }
  bool operator<(const dummy_state & other ) const
  {
    if ((*this).automatonState<other.automatonState)
      return true;
      else
	return false;
  }
};


struct monitor_result
{
	std::string agent_id;
	std::string behavior_name;
	bool ok;
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& behavior_name;
		ar& ok;
		ar& agent_id;
	}
	
};

struct monitor_packet
{
	std::string id;
	std::deque<monitor_result> agents;	
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar& agents;
		ar& id;
	}
	
};


struct graph_informations
{
    bool isNegotiating;
    std::vector<int> lockedNode;
   std::string priority; //priorita' durante l'handshake
    std::string id; //id dell'agente
    simulation_time timestamp; //Un giorno ci metteremo il tempo reale (dal 1970)
    bool emergency;
    simulation_time started_moving;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& timestamp;
        ar& isNegotiating;
        ar& lockedNode;
        ar& id;
	ar& priority;
	ar& emergency;
	ar& started_moving;
    }
    friend std::ostream& operator<< (std::ostream& os, const graph_informations& g)
    {
        os<<g.timestamp<<" "<<g.lockedNode<<" "<<g.id;
        return os;
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


struct agents_name_to_ref_states
//TODO(MIRKO): Controllare se questa struttura serve oppure può essere inclusa in World_sim_packet
{
  friend class boost::serialization::access;
  std::map<std::string,agent_state_packet*> internal_map;

private:
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar& internal_map;

    }
};



struct agent_simulator_handshake_packet{
  bool accepted;
  std::string message;
  
  template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
	ar& accepted;
        ar& message;
    }
  
};


struct world_sim_packet {
  
    std::map<std::string,double> bonus_variables;
    agents_name_to_states state_agents;
    simulation_time time;
    std::map<std::string, task_assignment_task> objects;
    
    template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
		ar& time;
        ar& bonus_variables;
        ar& state_agents;
	ar& objects;
    }
    
};

struct agent_sim_packet{
  //pacchetto personalizzato per ogni agente in base alla visibilita'
  const std::map<std::string,double>& bonus_variables;
   agents_name_to_ref_states state_agents;
  const simulation_time& time;
    
    agent_sim_packet(const std:: map<std::string,double>& bonus, const simulation_time& t)
    :bonus_variables(bonus),time(t)
    {
	
    }
    
    template <typename Archive>
    void serialize(Archive& ar,const unsigned int /*version*/)
    {
		ar& time;
        ar& bonus_variables;
        ar& state_agents;
    }
    
};

struct agent_sim_packet_receiver{
  //pacchetto personalizzato per ogni agente in base alla visibilita'
  std::map<std::string,double> bonus_variables;
   agents_name_to_ref_states state_agents;
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

class abstract_object 
{
public:
  //virtual abstract_object()=0;
  virtual const void* getState()=0;
  virtual bool isMovable()=0;
  virtual double getRadius()=0;
  virtual bool canCollide()=0;
  virtual void updateState(const simulation_time& time,const agents_name_to_states& agent_states,const index_map& agents_name_to_index)=0;
  virtual std::string getObjectType()=0;
  std::string name;
  virtual ~abstract_object()
  {
  };
};


#endif