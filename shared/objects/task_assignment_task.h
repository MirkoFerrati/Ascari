#ifndef TASK_ASSIGNMENT_TASK_H
#define TASK_ASSIGNMENT_TASK_H
#include "abstract_object.h"
#include "types.h"
#include "task_assignment_types.h"
#include "global_types.h"
#include "streams_utils.h"
#define TA_RADIUS 5


class task_assignment_task //:public abstract_object
{
public:
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
    
    inline const void* getState()
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
    
    //TODO: metodi propri del task_assignment_task
    
    	template <typename Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
	 	ar& object_type;
 		ar& state; 
	}
    
private:
  task_assignment_namespace::task state;
  std::string object_type;
};


#endif // TASK_ASSIGNMENT_TASK_H
