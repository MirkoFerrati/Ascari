#ifndef TASK_ASSIGNMENT_TASK_H
#define TASK_ASSIGNMENT_TASK_H
#include "streams_utils.h"
#include "types/agents_name_to_states.h"
#include "abstract_object.h"
#include "../../plugins/task_assignment/task_assignment_types.h"
#define TA_RADIUS 5

struct A;

class task_assignment_task :public abstract_object
{
  
        friend class boost::serialization::access;
template<class Archive>
void serialize(Archive &ar, const unsigned int /*version*/)
{
  ar & boost::serialization::base_object<abstract_object>(*this);
  ar& state;
  ar& object_type;
}
 
public: 
  task_assignment_namespace::task state;
  std::string object_type;
  

    inline bool canCollide()
    {
      return false;
    }
    inline double getRadius()
    {
      return TA_RADIUS;
    }
      inline bool isMovable()
    {
     return false; 
    }
    
    inline const void* getState() const
    {
    return &state; 
    }
  
    inline  std::string getObjectType()
    {
     return object_type; 
    }
    
    void updateState ( const simulation_time& time, const agents_name_to_states& agents, const index_map& indexes );
  
    task_assignment_task();
    
    task_assignment_task( const task_assignment_namespace::task& me );
  
    ~task_assignment_task();
    
    friend std::ostream& operator<<( std::ostream& os,const task_assignment_task& t)
    {
	os << std::endl;
	
	os<< "Name: "<<t.name<<std::endl;
	os<< "Type: "<<t.object_type;
	os<< t.state;
	
	os<< std::endl;
	return os;
    }
   
    virtual void print ( std::ostream& os ) const{os<<*this;};
  
};


#endif // TASK_ASSIGNMENT_TASK_H
