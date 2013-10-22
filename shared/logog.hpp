#ifdef AGENT_LIGHT


#define ERR(...) {}
#define WARN(...) {}
#define LOGOG_INITIALIZE(...) {}
#define LOGOG_SHUTDOWN(...) {}
#define INFO(...) {}
namespace logog
{
 typedef int Cout;
  
}

#else
#include <../shared/logog/include/logog.hpp>


#endif

