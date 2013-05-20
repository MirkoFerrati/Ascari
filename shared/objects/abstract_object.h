
#ifndef ABSTRACT_OBJECT_H
#define ABSTRACT_OBJECT_H
#include <types/agents_name_to_states.h>
#include <types.h>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/base_object.hpp>
#include <ostream>

class abstract_object 
{
  //virtual abstract_object()=0;
      friend class boost::serialization::access;
      template<class Archive>
        void serialize(Archive &ar, const unsigned int /*version*/)
  {
    ar& name;
  }
    std::string name;

public:

  virtual const void* getState() const=0;
  virtual bool isMovable()=0;
  virtual double getRadius()=0;
  virtual bool canCollide()=0;
  virtual void updateState(const simulation_time& time,const agents_name_to_states& agent_states,const index_map& agents_name_to_index)=0;
  virtual std::string getObjectType()=0;
  virtual ~abstract_object(){};
  
  virtual void print(std::ostream& os)const{};
  
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(abstract_object)

#endif //ABSTRACT_OBJECT_H