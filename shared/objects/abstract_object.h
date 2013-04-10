
#ifndef ABSTRACT_OBJECT_H
#define ABSTRACT_OBJECT_H

#include <string>
#include <typedefs.h>

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

#endif //ABSTRACT_OBJECT_H