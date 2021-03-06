#ifndef TASK_ASSIGNMENT_TYPES_H
#define TASK_ASSIGNMENT_TYPES_H

#include "streams_utils.h"

#include <map>
#include <string>
#include <vector>
#include <list>
#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/deque.hpp"
#include <exprtk.hpp>
#include "types.h"
#include <iostream>
#include <fstream>
#include <forward_list>


//written by Alessandro Settimi
namespace task_assignment_namespace
{
typedef double task_cost;
typedef std::string task_id;
typedef std::string agent_id;
typedef std::map<task_id,task_cost> task_cost_vector;
typedef std::map<agent_id,task_cost_vector> task_cost_matrix;
typedef std::map<task_id,double> task_assignment_vector;
typedef std::map<agent_id,task_assignment_vector> task_assignment_matrix;
typedef int task_assignment_algorithm;

struct task
{
    task_id id;
    double task_position[3];
    int task_type;
    double task_execution_time;
    double period;
    double task_deadline;
    bool executing;
    simulation_time time;
    agent_id owner;
    bool available;
    bool done;
    
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int /*version*/)
    {
      ar& id;
      ar& task_position;
      ar& task_type;
      ar& task_execution_time;
      ar& period;
      ar& task_deadline;
      ar& executing;
      ar& time;
      ar& owner;
      ar& available;
      ar& done;
    }
    
    friend std::ostream& operator<<( std::ostream& os,const task& t)
    {
	os << std::endl << "TASK " << t.id <<':'<< std::endl;
	os << "- posizione: " << t.task_position[0] <<' '<< t.task_position[1]<<' '<< t.task_position[2] << std::endl;
	os << "- tipo: " << t.task_type << std::endl;
	os << "- execution time: " << t.task_execution_time << std::endl;
	os << "- period: " << t.period<< std::endl;
	os << "- deadline: " << t.task_deadline << std::endl;
	os << "- executing: " << t.executing << std::endl;
	os << "- time: " << t.time << std::endl;
	os << "- owner: " << t.owner << std::endl;
	os << "- available: " << t.available << std::endl;
	os << "- done: " << t.done << std::endl;
	os<<std::endl;
	return os;
    }
};


typedef std::map<task_id,task> task_list;


template <typename data_type>
class task_assignment_packet
{
public:
    
    std::string agent_id;
    double x;
    double y;
    double theta;
    bool busy;
    task_id taken_task;
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
	ar& x;
	ar& y;
	ar& theta;
	ar& busy;
	ar& taken_task;
        ar& data;
    }
};

struct subgradient_task_packet
{
      std::vector<double> subgradient;
      
      template <typename Archive>
      void serialize(Archive& ar, const unsigned int /*version*/)
      {
	ar& subgradient;
      }
};

typedef task_assignment_packet<subgradient_task_packet> subgradient_packet;

}
#ifndef GLP_MIN
#define GLP_MIN 1
#endif

#ifndef GLP_MAX
#define GLP_MAX 2
#endif

#define INF 1000000.0

#define SUBGRADIENT 0
//written by Alessandro Settimi

#endif