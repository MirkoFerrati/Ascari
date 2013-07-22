// # Connection:
// #        M1 pin  -> Digital pin 4
// #        E1 pin  -> Digital pin 5
// #        M2 pin  -> Digital pin 7
// #        E2 pin  -> Digital pin 6
// #        Motor Power Supply -> Centor blue screw connector(5.08mm 3p connector)
// #        Motor A  ->  Screw terminal close to E1 driver pin
// #        Motor B  ->  Screw terminal close to E2 driver pin
// # 
// # Note: You should connect the GND pin from the DF-MD v1.3 to your MCU controller. They should share the GND pins.
// #


#include "CmdMessenger.h"
#include "motor_control.h"
#include <string.h>

#define LEFT 0
#define RIGHT 1
#define Tprint 1000
#define prn_enc 0
#define pwm_default 255

#define v_min -3
#define v_max 3
#define w_min -10
#define w_max 10

volatile int coder[2] = {0,0};
int lastSpeed[2] = {0,0};  

char field_separator = ',';
char command_separator = ';';         

int ledPin = 13;

int E2 = 6;
int M2 = 7;
int E1 = 5;                         
int M1 = 4;                           
unsigned long timer = 0;                //print manager timer

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);

// Commands we send from the Arduino to be received on the PC
enum
{
  kCOMM_ERROR    = 000, // Lets Arduino report serial port comm error back to the PC (only works for some comm errors)
  kACK           = 001, // Arduino acknowledges cmd was received
  kARDUINO_READY = 002, // After opening the comm port, send this cmd 02 from PC to check arduino is ready
  kERR           = 003, // Arduino reports badly formatted cmd, or cmd not recognised

  // Now we can define many more 'send' commands, coming from the arduino -> the PC, eg
  // kICE_CREAM_READY,
  // kICE_CREAM_PRICE,
  // For the above commands, we just call cmdMessenger.sendCmd() anywhere we want in our Arduino program.

  kSEND_CMDS_END, // Mustnt delete this line
};

void stop_motors();
void gest_motors();
void led_blinking();
void v2pwm_cmd();

int pwm_serial();
void set_pwm(int value_left,int value_right);
void ferma();
double get_v();
double get_w();
void vw2pwm(double v, double w);
void v2pwm(double vl, double vr);

messengerCallbackFunction messengerCallbacks[] = 
{
  gest_motors,		// 004 in this example
  stop_motors,
  led_blinking,
  v2pwm_cmd,
};

// ------------------ C A L L B A C K  M E T H O D S -------------------------


void gest_motors()
{
  // Message data is any ASCII bytes (0-255 value). But can't contain the field
  // separator, command separator chars you decide (eg ',' and ';')
  //cmdMessenger.sendCmd(kACK,"cmd received");
  if ( cmdMessenger.available() )
  {
        set_pwm(pwm_serial(),pwm_serial());
      
    }
}


void stop_motors()
{ 
        ferma();
}

void led_blinking()
{
  double time_on, time_off;

  if ( cmdMessenger.available() )
  {
       	char buf[350] = { '\0' };
       	cmdMessenger.copyString(buf, 350);
	time_on = atof(buf);
	if (time_on < 0)
		time_on = 0;
	else if (time_on > 255)
		time_on = 255;

	cmdMessenger.copyString(buf, 350);
	time_off = atof(buf);
	if (time_off < 0)
		time_off = 0;
	else if (time_off > 255)
		time_off = 255;

	digitalWrite(13, HIGH);
	delay(10*time_on);
	digitalWrite(13, LOW);
	delay(10*time_off);	
  	
	//Serial.print("Command EXECUTED:");
    //Serial.print(time_on);
    //Serial.print(",");
    //Serial.println(time_off);
	}	
}
 
void v2pwm_cmd()
{
	if ( cmdMessenger.available() )
    {
		double v = get_v();
		double w = get_w();
        vw2pwm(v,w);
    }
} 
 
 // ------------------  M E T H O D S -------------------------

void set_pwm(int value_left,int value_right){
	digitalWrite(M2,HIGH); 
	if (value_left <0)
		digitalWrite(M2,LOW);
	digitalWrite(M1,LOW);
	if (value_right <0)
                digitalWrite(M1,HIGH);
	
 	analogWrite(E2, abs(value_left));
      	analogWrite(E1, abs(value_right));

    //  Serial.print("Command EXECUTED:");
    //  Serial.print(value_right);
    //  Serial.print(",");
    //  Serial.println(value_left);
}


void ferma(){
  analogWrite(E1, 0);  
  analogWrite(E2, 0);
//  Serial.println("Command EXECUTED: STOP");
}


int pwm_serial(){
       int pwm_cur; 
       pwm_cur= pwm_default;
       if (cmdMessenger.available()){
         char buf[350] = { '\0' };
        cmdMessenger.copyString(buf, 350); 
        pwm_cur=atoi(buf);
       }
       if (pwm_cur< -pwm_default)
       pwm_cur= -pwm_default;
	
	if (pwm_cur > pwm_default)
       pwm_cur= pwm_default;

       return pwm_cur;
}

void LwheelSpeed()
{
  coder[LEFT]++;  //count the left wheel encoder interrupts
}


void RwheelSpeed()
{
  coder[RIGHT]++; //count the right wheel encoder interrupts
}  


double get_v(){
       double v; 
       v = 0;
       if (cmdMessenger.available()){
			char buf[350] = { '\0' };
			cmdMessenger.copyString(buf, 350); 
			v=atof(buf);
       }
       if (v < v_min)
		v = v_min;
	
	if (v > v_max)
       v= v_max;

       return v;
}

double get_w(){
       double w; 
       w = 0;
       if (cmdMessenger.available()){
			char buf[350] = { '\0' };
			cmdMessenger.copyString(buf, 350); 
			w=atof(buf);
       }
       if (w < w_min)
		w = w_min;
	
	if (w > w_max)
       w= w_max;

       return w;
}

void vw2pwm (double v, double w)
{
	double b = 0.09;
	double vl = v;
	double vr = v+(w)*b;
	
	v2pwm(vl, vr) ;
}

void v2pwm(double vl, double vr)
{
	int vl_sign = vl>0 ? 1 : -1;
	double vl_abs = abs(vl);
	int vr_sign = vr>0 ? 1 : -1;
	double vr_abs = abs(vr);
	int left = 0;
	int right = 0;

	
	/* LEFT */
	if (vl_abs < 0.082)
	{
		left = 0;
	}
	else if (vl_abs < 0.41)
	{
		left  = vl_sign*(55+(vl_abs-0.082)/0.0033);
	}
	else if (vl_abs < 0.5467)
	{
		left  = vl_sign*(155+(vl_abs-0.41)/0.0014);
	}
	else
	{
		left  = vl_sign*255;
	}
	
	/* RIGHT */
	if (vr_abs < 0.082)
	{
		right = 0;
	}
	else if (vr_abs < 0.41)
	{
		right = vr_sign*(55+(vr_abs-0.082)/0.0033);
	}
	else if (vr_abs < 0.5467)
	{
		right = vr_sign*(155+(vr_abs-0.41)/0.0014);
	}
	else
	{
		right = vr_sign*255;
	}
	
	set_pwm(right, left);
}

// ------------------ D E F A U L T  C A L L B A C K S -----------------------

void arduino_ready()
{
  // In response to ping. We just send a throw-away Acknowledgement to say "im alive"
  //cmdMessenger.sendCmd(kACK,"Arduino ready");
}

void unknownCmd()
{
  // Default response for unknown commands and corrupt messages
  //cmdMessenger.sendCmd(kERR,"Unknown command");
}

// ------------------ E N D  C A L L B A C K  M E T H O D S ------------------



// ------------------ S E T U P ----------------------------------------------

void attach_callbacks(messengerCallbackFunction* callbacks)
{
  int i = 0;
  int offset = kSEND_CMDS_END;
  while(callbacks[i])
  {
    cmdMessenger.attach(offset+i, callbacks[i]);
    i++;
  }
}

void setup() 
{
  // Listen on serial connection for messages from the pc
  // Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial
  Serial.begin(115200); // Arduino Uno, Mega, with AT8u2 USB

  // cmdMessenger.discard_LF_CR(); // Useful if your terminal appends CR/LF, and you wish to remove them
  cmdMessenger.print_LF_CR();   // Make output more readable whilst debugging in Arduino Serial Monitor
  
  // Attach default / generic callback methods
  cmdMessenger.attach(kARDUINO_READY, arduino_ready);
  cmdMessenger.attach(unknownCmd);
 
  // Attach my application's user-defined callback methods
  attach_callbacks(messengerCallbacks);
  pinMode(3, INPUT);
  pinMode(2, INPUT);
  digitalWrite(3, HIGH);
  digitalWrite(2, HIGH);
  attachInterrupt(LEFT, LwheelSpeed, CHANGE);    //init the interrupt mode for the digital pin 2
  attachInterrupt(RIGHT, RwheelSpeed, CHANGE);   //init the interrupt mode for the digital pin 3 

  arduino_ready();
  
  pinMode(M1, OUTPUT);   
  pinMode(M2, OUTPUT);

  pinMode(13, OUTPUT);
}


// ------------------ M A I N ( ) --------------------------------------------

void loop() 
{
  // Process incoming serial data, if any
  cmdMessenger.feedinSerialData();
  
  

  //if( ((millis() - timer) > Tprint) && prn_enc==1 ){                   
    //Serial.print("Elapsed time: ");
    //Serial.print(millis()-timer);    

//    lastSpeed[LEFT] = coder[LEFT];   //record the latest speed value
 //   lastSpeed[RIGHT] = coder[RIGHT];
  //  coder[LEFT] = 0;                 //clear the data buffer
  //  coder[RIGHT] = 0;
    //timer = millis();
    //delay(10);
  //}
  // Loop.
}


