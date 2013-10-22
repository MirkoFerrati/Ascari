#include <condition_variable>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include "serial_communicator.hpp"
#include "raw_serial.hpp"


//#include "time.h"



using namespace std;

int cycle_period_millisec;
bool s_interrupted;
raw_serial communicator;
volatile bool next_one;

void initialize ( )
{
    cycle_period_millisec=0;//250;    
    s_interrupted=false;
}

void input_loop ( mutex& input_mutex,condition_variable& input_cond,volatile bool& paused,volatile bool& exit,volatile int& cycle_period_millisec )
{
    char c;
    string temp;
    bool isReading;
    while ( !exit )
    {
        std::cin.read ( &c,1 );
        if ( isReading )
        {
            temp.push_back ( c );
        }

	if (c=='q')
	{
	    exit=true;
            s_interrupted=true;
	}

	if (c=='w'||c=='a'||c=='s'||c=='d'||c=='i'||c=='k')
	{
	    std::lock_guard<std::mutex> lock (input_mutex);
	    communicator.setControls(c);
	    next_one=true;
	}

        if ( c=='p' )
        {
            std::lock_guard<std::mutex> lock ( input_mutex );
            paused=!paused;
            c='0';
            input_cond.notify_one();
        }
        if ( c=='l' )
        {
            isReading=true;
            temp.clear();
            c='0';
        }
        if ( c=='!' )
        {
            isReading=false;
            std::lock_guard<std::mutex> lock ( input_mutex );
            cout<<"letto un carattere:"<<c<<temp<<atoi ( temp.c_str() ) <<endl;
            cycle_period_millisec=atoi ( temp.c_str() );
            c='0';
            input_cond.notify_one();
        }
    }
}


void main_loop()
{
    try
    {
        termios before, after;
        tcgetattr ( STDIN_FILENO, &before ); // fill 'before' with current termios values
        after = before;                     // make a copy to be modified
        after.c_lflag &= ( ~ICANON );       // Disable canonical mode, including line buffering
        after.c_lflag &= ( ~ECHO );         // Don't echo characters on the screen (optional)
        tcsetattr ( STDIN_FILENO, TCSANOW, &after ); // Set the modified flags
	cout<<"started"<<endl;
        std::mutex input_mutex;
        std::condition_variable input_cond;
        volatile bool input_exit=false;
        volatile bool paused=false;
        std::thread input_thread ( &input_loop,std::ref ( input_mutex ),std::ref ( input_cond ),std::ref ( paused ),std::ref ( input_exit ),std::ref ( cycle_period_millisec ) );
        struct timespec requestStart, requestEnd, simulationStart;
        clock_gettime ( CLOCK_REALTIME, &simulationStart );
        double total_accum, step_accum;
        while ( !s_interrupted )
        {
            clock_gettime ( CLOCK_REALTIME, &requestStart );

            //Check for input loop
            {
                sleep ( 0 );
                std::unique_lock<std::mutex> lock ( input_mutex );
                while ( paused )
                {
                    input_cond.wait ( lock );
                    // cout<<paused<<endl;
                }
            }

            // Calculate time taken by a request
            clock_gettime ( CLOCK_REALTIME, &requestEnd );

//             Calculate time it took
            step_accum = ( requestEnd.tv_sec - requestStart.tv_sec )*1000
                    + ( requestEnd.tv_nsec - requestStart.tv_nsec )/ 1E6;

	    if (next_one)
	    {
	    	communicator.send();
		next_one=false;
	    }
            if ((cycle_period_millisec-step_accum)*1000-10 >0)
	    {
              //  usleep ( (cycle_period_millisec-step_accum)*1000-10 );
	    }
	    else
	    {
	//	WARN("this timestep %f took too much time: %f",sim_packet.time,step_accum);
	    }
        }

        input_exit=true;
        tcsetattr ( STDIN_FILENO, TCSANOW, &before );

    }
    catch ( const char* e )
    {

    }
}





int main ( int argc, char **argv )
{

    srand ( time ( NULL ) );
    initialize();
    main_loop();
    return 0;
}

