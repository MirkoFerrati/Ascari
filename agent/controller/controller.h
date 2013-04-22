#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <string>



#include <vector>
#include "../shared/exprtk.hpp"

class controller
{

public:
    /**
      * Il controllore inizializza le espressioni usando l'indirizzo della variabile state,
      * bisogna essere certi che l'indirizzo non cambi mai durante l'esecuzione
      */
    controller(std::map<int,std::string>const& map_id_expressions, std::vector<std::string> const& variables,
			   exprtk::symbol_table<double>& symbol_table
  			);

    /** Calcola il valore dell'azione di controllo attuale sulla base dello stato continuo 
    * 	ottenuto dall'indirizzo passato al costruttore
    */
    void computeControl();

private:
    //exprtk::symbol_table<double> symbol_table;
    std::vector< exprtk::expression<double> > expressions;
};

#endif // CONTROLLER_H
