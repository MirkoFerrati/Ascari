#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "exprtk.hpp"
#include "types.h"
#include "define.h"
#include "dynamic_module_abstract.h"
#include <vector>
#include <map>
#include <string>


class dynamic : public dynamic_module_abstract
{
public:
    /**
     * Questo costruttore non modifica in alcun modo \param StateReferenceWARN e \param controlReferenceWARN,
     * ma purtroppo la libreria exprtk non vuole const&, perciò siamo costretti a passarci & normali
     */
    //TODO(Mirko): risolvere questo problema, creare una versione non-const di getNextState() che non ritorna nulla e modifica direttamente lo stato,
    // e lasciare questa versione qui solo per casi speciali
	
	dynamic(agent_state& StateReferenceWARN,control_command& controlReferenceWARN, 
		std::map< std::string, std::string > expression, std::vector< std::string > state_variables_name, 
	 std::vector< std::string > control_variables_name, float tcamp=T_CAMP);
	
	
	agent_state getNextState();
	 ~dynamic();
	
	
private:
	std::vector<exprtk::expression<double> > expressions;
	exprtk::symbol_table<double> symbol_table;
	std::map<std::string,int> map_expressions;
	const agent_state& StateReferenceWARN;
	float T_camp;
	dynamic(const dynamic& d);
};

#endif // DYNAMIC_H
