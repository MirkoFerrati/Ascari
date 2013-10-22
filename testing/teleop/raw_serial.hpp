#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ostream>
#include <stdio.h>
#include <fcntl.h>


using namespace std;

class raw_serial
{
public:
int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
         //       error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
       //         error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
          abort();//      error_message ("error %d setting term attributes", errno);
}

int fd;
std::string command;
double v,w,sign=0;
void setControls(char c)
{
     if (c=='i')
        {
                v=v+0.1;
        }
        if (c=='a')
        {
                w=w+0.1;
        }
	if (c=='w')
	{
		if (v<-0.5)
			v=v+0.3;
		else
			sign=1;
	}
	if (c=='s')
	{
		if (v>0.5)
			v=v-0.3;
		else
			sign=-1;
	}
        if (c=='k')
        {
            if (v>=0)
                  v=v-0.1;
            else v=0;

        }
        if (c=='d')
        {
                w=w-0.1;
	}
      cout << "SerialMessage Sent: " <<7<<","<<sign*v<<","<<w<<";"<<endl;
char buf[100];
sprintf(buf,"7,%f,%f;",sign*v,w);
command=buf;


}

void send()
{
  write(fd,command.c_str(),command.length());
  cout<< "Sent: "<<command<<endl;
}

raw_serial()
{
string portname = "/dev/ttyACM0";
v=w=0;
command="7,0,0;";
fd = open (portname.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
if (fd < 0)
{
      //  error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
        return;
}

set_interface_attribs (fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
set_blocking (fd, 0);                // set no blocking

write (fd, "1;2;", 4);           // send 7 character greeting
char buf [100];
int n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
std::cout<<buf<<endl;
}

};
