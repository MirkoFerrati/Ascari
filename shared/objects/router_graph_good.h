#ifndef ROUTER_GRAPH_GOOD_H
#define ROUTER_GRAPH_GOOD_H
#include "abstract_object.h"

class router_graph_good : public abstract_object
{
public:
    router_graph_good();
    
    
    virtual bool canCollide();
    
    virtual std::string getObjectType();
    
    virtual double getRadius();
    
    virtual const void* getState() const;
    
    virtual bool isMovable();
    
    virtual void updateState ( const simulation_time& time, const agents_name_to_states& agent_states, const index_map& agents_name_to_index );
    
     ~router_graph_good();
  
  
};

#endif // ROUTER_GRAPH_GOOD_H
