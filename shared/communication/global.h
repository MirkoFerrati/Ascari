#ifndef GLOBALCONTEXT_H
#define GLOBALCONTEXT_H
#include <signal.h>

#include <zmq.hpp>
#include <iostream>
namespace static_zmq
{
	extern zmq::context_t context;
}


extern int s_interrupted;
static bool catching_signals=false;
static void s_signal_handler(int /*signal_value*/)
{
 s_interrupted=1; 
 std::cout<<"interrotto"<<std::endl;
}

inline void s_catch_signals (void)
{
  if (!catching_signals)
  {
	catching_signals=true;
	signal(SIGABRT, &s_signal_handler);
	signal(SIGTERM, &s_signal_handler);
	signal(SIGINT, &s_signal_handler);
  }
}



#endif //GLOBALCONTEXT_H
