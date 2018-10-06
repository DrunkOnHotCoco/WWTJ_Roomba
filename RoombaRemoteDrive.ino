//Main program for Xbee Communication & Roomba Control
//Loosely based on the principals of SerialCTL written by Nash Kaminski
//By learning and experimenting with the principals of the program, future ITR members will gain a strong base of understanding
//for the SerialCTL program


#include <SoftwareSerial.h>
//Roomba Command Codes
int START = 128;        //Starts Serial Control Interface (SCI)
int ENABLE_CONTROL = 130;   //Enables user control of Roomba (must be sent after the start command and every control command)
int FULL_MODE = 132;               //Enables unrestricted use of Roomba Commands (no safety features!)
int LED_CONTROL = 139;          //Control command for Status LED
int DRIVE = 137;
//roombaRX
int roombaRX = 10, roombaTX = 11; //these are the pins that will transmit and recieve data from the Roomba
SoftwareSerial roombaSerial(roombaRX,roombaTX);

char Xaxis = 0; //This value represents the recieved X value (0 to 30)
char Yaxis = 0; //This value represents the recieved Y value
bool button_1 = 0;
bool button_2 = 0;
bool button_3 = 0;
bool button_4 = 0;
int inNum; //This holds the recieved byte from the Xbee

bool Active = true;

void setup() {  
  Serial.begin(57600);          //this Serial line will read data from the xbee
  roombaSerial.begin(57600);    //this Serial line will send commands to the roomba

  roombaSerial.write(128); //These two bytes initialize the roomba
  delay(50);
  roombaSerial.write(130);
  delay(50);  
  roombaSerial.write(132); //This sets the Roomba in 'unsafe' mode
  delay(50);
  roombaSerial.write(139); //LEDS
  roombaSerial.print(0);
  roombaSerial.print(1);
  roombaSerial.write(255);
  
  }

void loop() {
  //The laptop-xbee-remote combination, or control station, will transmit bytes to the roomba that contain information on the state of the remote

  if(Serial.available() > 2){ //if there is new data from the control station, interpret it
    
    Xaxis = Serial.read();  //get the value
    Yaxis = Serial.read();
    button_1 = Serial.read();
    Serial.print("X Axis:  ");
    Serial.print(Xaxis);
    Serial.print("Y Axis: ");
    Serial.print(Yaxis);
    Serial.print("Button 1:  ");
    Serial.print(button_1);
    Serial.println();
    if(Xaxis == 255){
        Active = false;
        roombaSerial.write(139); //LEDS
        roombaSerial.print(0);
        roombaSerial.print(255);
        roombaSerial.write(255);
    }    
    if(Yaxis == 255){
        Active = true;
        roombaSerial.write(139); //LEDS
        roombaSerial.print(0);
        roombaSerial.print(1);
        roombaSerial.write(255);
    }    
    
    if(Active){
    driveRoomba(Xaxis, Yaxis, 13,17); //This method takes the current X and Y value of the remote and translates it into Roomba motion.
    }
     //roombaSerial.write(ENABLE_CONTROL);
     //delay(50);
    //roombaSerial.write(128); //These two bytes initialize the roomba
  //delay(50);
  roombaSerial.write(130);
  delay(50);  
  roombaSerial.write(132); //This sets the Roomba in 'unsafe' mode
  }

}

void driveRoomba(int joyX, int joyY, int boundsLow, int boundsHigh)
{
  //This function uses input values to move the robot
  //Then sends an instruction to the robot
  
  int speed = map(joyY, 0,30, -500,500);  //The Y value acts as the power to the motors
  int radius = map(joyX,0,30, -2000,2000); //the X value acts as the radius for the robots turn (the further from the center of the joystick the smaller the radius)
  

  //for example to move straight, you simply move the Y axis up 
  //0,0 for the joysticks is in the top left corner

  

  
  if(joyX < boundsLow) //these functions invert the radius (remember further from center the smaller the radius so tighter turns)
  {
    radius = map(joyX,0,15,-1,-200);
  }
  if(joyX > boundsHigh)
  {
    radius = map(joyX,15,30,200,0);
  }
  
  if(joyX > boundsLow && joyX < boundsHigh) //if the stick is in the center (based on given bounds) then we want to set it to 0
  {
     radius = 32768;//this is the given 0 value for the radius, part of the documentation
  }
  if(joyY > boundsLow && joyY < boundsHigh)
  {
    speed = 0;
  }
  //Status of Joystick thing
 //roombaSerial.print("Speed:  "  + speed);
 //roombaSerial.print("Radius: " +  radius);
  
  roombaSerial.write(137);      //This line sends the instructions over serial
  roombaSerial.write((speed >> 8) & 255);
  roombaSerial.write((speed) & 255);
  roombaSerial.write((radius >> 8) & 255);
  roombaSerial.write((radius) & 255);
  
   
  
}

