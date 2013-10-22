
#include <SerialStream.h>
#include <iomanip>
#define ARDUINO_COMMAND_CODE 7

using namespace std;
using namespace LibSerial;


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

class serial_communicator
{
private:

double v,w;
LibSerial::SerialStream serial_port;


public:
serial_communicator()
{
	
    v=w=0;

    /* Serial Port to Arduino initialization */
    
    serial_port.Open("/dev/ttyACM0");
    assert(serial_port.good());

    serial_port.SetBaudRate( SerialStreamBuf::BAUD_115200 );
    assert(serial_port.good());

    serial_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 );
    assert(serial_port.good());

    serial_port.SetParity( SerialStreamBuf::PARITY_NONE );
    assert(serial_port.good());

    serial_port.SetNumOfStopBits( 1 );
    assert(serial_port.good());

    serial_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE );
    assert(serial_port.good());

    std::string temp;
    serial_port << 1<<";";
    serial_port>>temp;
    cout<<temp<<endl;
    serial_port << 2<<";";
    serial_port>>temp;
    cout<<temp<<endl;
sleep(1);    
    
}


void setControls(char c)
{
	if (c=='w')
	{
		v=v+0.1;
	}
	if (c=='a')
	{
		w=w+0.1;
	}

	if (c=='s')
	{
		v=v-0.1;
	}
	if (c=='d')
	{
		w=w-0.1;
	}
}


void send()
{
    DECLARE_TIMING(myTimer);
        START_TIMING(myTimer);
        serial_port << setprecision(6)<<ARDUINO_COMMAND_CODE<<","<<v<<","<<w<<";";
	serial_port.flush();
        cout << "SerialMessage Sent: " <<ARDUINO_COMMAND_CODE<<","<< v<<","<<w<<";"<<endl;
        STOP_TIMING(myTimer);
        printf("Execution time: %f ms.\n", GET_TIMING(myTimer) );
        printf("Average time: %f ms per iteration.\n", GET_AVERAGE_TIMING(myTimer) );
}


};
