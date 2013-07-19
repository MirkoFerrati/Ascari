#ifndef OBJECTS_CONTAINER_HPP
#define OBJECTS_CONTAINER_HPP
#include <boost/serialization/access.hpp>
#include <list>
#include "abstract_object.h"
#include "task_assignment_task.h"
#include "router_graph_good.h"
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>

class objects_container
{
  friend class boost::serialization::access;
 
  
  template<class Archive>
  void serialize(Archive& ar, const unsigned int /*version*/)
  {
    //ar.register_type(static_cast<task_assignment_task*>(NULL));
    //ar.register_type(static_cast<router_graph_good*>(NULL));
    ar& objects;
  }
  
public:
   std::map<std::string, std::list<abstract_object*>> objects;
  
};

#endif // OBJECTS_CONTAINER_HPP
