#ifndef TASK_ASSIGNMENT_TASK_H
#define TASK_ASSIGNMENT_TASK_H
#include "abstract_object.h"
#include "typedefs.h"

#define RADIUS 5

class task_assignment_task 
{
public:
    inline bool canCollide()
    {
      return false;
    }
    inline double getRadius()
    {
      return RADIUS;
    }
      inline bool isMovable()
    {
     return false; 
    }
    
    inline const void* getState()
    {
     return &state; 
    }
  
    inline virtual std::string getObjectType()
    {
     return "TA_task"; 
    }
    
    void updateState ( const simulation_time& time, const agents_name_to_states& agent_states );
  
    task_assignment_task(TODO);
  
    //TODO: metodi propri del task_assignment_task
    
private:
  task_assignment_namespace::task state;
};

#endif // TASK_ASSIGNMENT_TASK_H
