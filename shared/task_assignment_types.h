#ifndef TASK_ASSIGNMENT_TYPES_H
#define TASK_ASSIGNMENT_TYPES_H

#include "types.h"
#include "streams_utils.h"

#include <map>
#include <string>
#include <vector>
#include <list>
#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/deque.hpp"
#include <exprtk.hpp>
#include "objects/abstract_object.h"
#include "objects/task_assignment_task.h"
#include "objects/task_assignment_task.h"
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
typedef std::map<task_assignment_namespace::agent_id,task_assignment_namespace::task_cost_vector> task_cost_matrix;
typedef std::map<task_id,double> task_assignment_vector;
typedef std::map<task_assignment_namespace::agent_id,task_assignment_namespace::task_assignment_vector> task_assignment_matrix;
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