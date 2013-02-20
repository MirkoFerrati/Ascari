#include <yaml_checker/yaml_checker.h>
#include <time.h>
#include <iostream>
#include <string>
#include "agent.h"
#include "../shared/logog/include/logog.hpp"
#include "../shared/typedefs.h"
#include "../shared/communication/global.h"
#include "lemon/arg_parser.h"


int main(int argc, char *argv[])
{
 srand (time (NULL));
    LOGOG_INITIALIZE();
    {
        logog::Cout out;
        lemon::ArgParser ap (argc, argv);
        std::string filename;
        ap.refOption ("f", "Yaml filename", filename, true);
        ap.synonym ("filename", "f");
        ap.throwOnProblems();
        try {
            ap.parse();
        } catch (lemon::ArgParserException const& ex) {
            ERR ("errore nella lettura dei parametri %s", ex.reason());
            return 0;
        }
        std::cout << "Parameters of '" << ap.commandName() << "':\n";
        std::cout << "  Value of -f: " << filename << std::endl;

	Yaml_checker checker;
	checker.checkYaml(filename);
    }
    LOGOG_SHUTDOWN();
    
    return 0;
 
  
}
