#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <string>

#include "typedefs.h"

class controller
{
	public:
		/**
		 * Il controllore inizializza le espressioni usando l'indirizzo della variabile state,
		 * bisogna essere certi che l'indirizzo non cambi mai durante l'esecuzione
		 */
    controller(std::map<int,std::string> control_expressions, agent_state& state_reference_WARN);

	/** Calcola il valore dell'azione di controllo attuale sulla base dello stato continuo 
	* 	ottenuto dall'indirizzo passato al costruttore
	*/
    control_command computeControl();

private:
	

};

#endif // CONTROLLER_H
