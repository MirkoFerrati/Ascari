#ifndef GRAPH_INFORMATIONS_H
#define GRAPH_INFORMATIONS_H
#include <types.h>

#include <string>
#include <vector>
#include <map>
#include "streams_utils.h"
#include "types.h"
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

typedef std::map<std::string,graph_informations> graph_packet;

#endif //graph_informations_h