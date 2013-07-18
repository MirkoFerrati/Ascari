

#include "../shared/communication/zmq_localization_communicator.hpp"
#include "lemon/arg_parser.h"
#include "../shared/yaml_parser.h"

#include "../shared/communication/global.h"


int64_t get_tick_count()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (int64_t)tp.tv_sec*1000000000 + tp.tv_nsec;
}

#define DECLARE_TIMING(s)  int64_t timeStart_##s; double timeDiff_##s; double timeTally_##s = 0; int countTally_##s = 0
#define START_TIMING(s)    timeStart_##s = get_tick_count()
#define STOP_TIMING(s) 	   timeDiff_##s = (double)(get_tick_count() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s) 	   (double)(timeDiff_##s / ( 1e9*1000.0))
#define GET_AVERAGE_TIMING(s)   (double)(countTally_##s ? timeTally_##s/ ((double)countTally_##s *  1e9*1000.0) : 0)
#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0






void sendToSimulator(std::string agent_name, agent_state state,zmq_localization_communicator_sender& communicator)
{
    agent_state_packet_from_webcam temp;
    temp.data.identifier=agent_name;
    temp.data.state=state;
    std::cout << "Agent "<<temp.data.identifier<<" "<<temp.data.state; //<< id << " x: " << x << " y: " << y << " theta: " << theta << std::endl;
    temp.webcam_id="fake_webcam";
    communicator.send(temp);

}


void internalNewCoord(int temp,double& x,double& y)
{
  temp=temp%300;
  double i=temp;
  if (i<100)
  {
    y=5;
    x=i/100.0*10.0;
  }
  else if (i<150)
  {
    x=10;
    y=5.0-i/50.0*5.0;
    
  }
  else if(i<250)
  {
    y=0;
    x=10.0-i/100.0*10.0;
    
  }
  else if(i<300)
  {
    x=0;
    y=i/50.0*5.0;
  }  
}

void getNewCoord(int i,double& x,double& y,double& theta)
{
  internalNewCoord(i,x,y);
  double tempx,tempy;
  internalNewCoord(i+40,tempx,tempy);
  theta=atan2(tempy-y,tempx-x);
  
}

int main ( int argc, char** argv )
{
   srand((unsigned)time(0));
      LOGOG_INITIALIZE();

    static_zmq::context=new zmq::context_t ( 1 );
    {
        logog::Cout out;
	zmq_localization_communicator_sender communicator;

    lemon::ArgParser ap ( argc, argv );
    int count,offsetx,offsety,maxdelta;
    std::string agent_name;
          std::string filename;
	bool filename_obbl=true;
	if (CONFIG.exists("FILENAME"))
	  filename_obbl=false;
        ap.refOption ( "f", "Yaml filename", filename, filename_obbl );
        ap.synonym ( "filename", "f" );
    ap.intOption ( "c", "Number of packets to send", 100 );
    ap.intOption("offsetx","offset error on x axis, default: randomized", -25+int(50.0*rand()/(RAND_MAX + 1.0)),false);
    ap.intOption("offsety","offset error on y axis, default: randomized", -25+int(50.0*rand()/(RAND_MAX + 1.0)),false);
    ap.intOption("maxdelta","maximum variable error on position, default: 5", 5,false);
    ap.refOption ( "a", "Agent name", agent_name, true );
    ap.synonym ( "agent", "a" );
    ap.throwOnProblems();
    try
    {
        ap.parse();
    }
    catch ( lemon::ArgParserException const& ex )
    {
        printf ( "errore nella lettura dei parametri %i", ex.reason() );
        return 0;
    }
    count=ap["c"];
    offsetx=ap["offsetx"];
    offsety=ap["offsety"];
      maxdelta=ap["maxdelta"];
      if (ap.given("filename"))
	  std::cout << "  Value of -f: " << filename << std::endl;

	if (!ap.given("filename"))
	{
	  filename=CONFIG.getValue("FILENAME");
  	  printf("Using %s as filename, read from config file",filename.c_str());
	}
	
	 yaml_parser parser;
       auto  world = parser.parse_file ( filename );
    communicator.init("fake_webcam");

    DECLARE_TIMING(myTimer);
    agent_state state;
    std::map<std::string,int> agent_state_index;
    int i=0;
    for (auto it=world.agents.front().behavior->state.begin();it!=world.agents.front().behavior->state.end();++it)
    {
        agent_state_index[*it]=i++;
    }
    
    double x,y;
    double theta;
    for (int i=0;i<count;i++)
    {
	getNewCoord(i,x,y,theta);
      
        START_TIMING(myTimer);
	state[agent_state_index["X"]]=x*100+offsetx-maxdelta/2.0+int(maxdelta*rand()/(RAND_MAX + 1.0));
	state[agent_state_index["Y"]]=y*100+offsety-maxdelta/2.0+int(maxdelta*rand()/(RAND_MAX + 1.0));;
	state[agent_state_index["THETA"]]=theta;
	
	sendToSimulator(agent_name,state,communicator);
	
        STOP_TIMING(myTimer);
        printf("Execution time: %f ms.\n", GET_TIMING(myTimer) );
    }
    printf("Average time: %f ms per iteration.\n", GET_AVERAGE_TIMING(myTimer) );
    }

}
