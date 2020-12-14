#include <Servo.h>
#include <math.h>
const int ledPin = 13; // the pin that the LED is attached to
const byte buffSize = 40;
unsigned int inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
byte coordinates[3];
int baseHomePosition = 120;
int shoulderHomePosition = 3;
int elbowHomePosition = 5;
double baseAngleRotationDegrees = 0;

Servo baseServo;
Servo shoulderServo;
Servo elbowServo;

void setup() {
// put your setup code here, to run once:
Serial.begin(115200);
pinMode(ledPin, OUTPUT);

baseServo.attach(9);
shoulderServo.attach(10);
elbowServo.attach(11);
delay(4000);

armToHomePosition();
}
void loop() 
{
  getDataFromPC();
  if(newDataFromPC)
  {
    sendCoordinatesToPC();
    // function to control motors goes here!
    double x = coordinates[0]* .5;
    double y = coordinates[1]* .5;

    double baseAngleRotationDegrees =  180-(int(atan2(y,x)*(180.0/PI))+90); // replace 60 with formula to determine base angle
    int shoulderAngleRotationDegrees = 0; // replace 0 with the formula to determine shoulder angle
    int elbowAngleRotationDegrees = 0; // replace 0 with the formula to determine elbow angle
    // rotate base:
    if(baseHomePosition < baseAngleRotationDegrees)
    {
      for (int angle = baseHomePosition; angle <= baseAngleRotationDegrees; angle += 1) {
      baseServo.write(angle);
      delay(15);
    }
    } else
    {
      for (int angle = baseHomePosition; angle >= baseAngleRotationDegrees; angle -= 1) 
    {
      baseServo.write(angle);
      delay(15);
    }
}



  for (int angle = shoulderHomePosition; angle <= shoulderAngleRotationDegrees; angle += 1) 
  {
    shoulderServo.write(angle);
    delay(15);
  }else 
  {
  for (int angle = shoulderHomePosition; angle >= shoulderAngleRotationDegrees; angle -= 1) 
  {
    shoulderServo.write(angle);
    delay(15);
  }
}
if(elbowHomePosition < elbowAngleRotationDegrees)
{
  for (int angle = elbowHomePosition; angle <= elbowAngleRotationDegrees; angle += 1) 
  {
    elbowServo.write(angle);
  }
  delay(15);
  }
  }else 
  {
    for (int angle = elbowHomePosition; angle >= elbowAngleRotationDegrees; angle -= 1) 
    {
      elbowServo.write(angle);
      delay(15);
    }
}

delay(1000);
digitalWrite(ledPin, HIGH);
delay(5000);
digitalWrite(ledPin, LOW);

sendSuspendCmd();

//sendEnableCmd();
newDataFromPC = false;
}
}
void sendSuspendCmd()
{
  // send the suspend-true command
  Serial.println("<S1>");
}
void sendEnableCmd()
{
  // send the suspend-false command
  Serial.println("<S0>");
}
void armToHomePosition()
{
  shoulderServo.write(shoulderHomePosition);
  delay(2000);
  baseServo.write(baseHomePosition);
  delay(2000);
  elbowServo.write(elbowHomePosition);
  delay(2000);
}
void armToShape()
{
  shoulderServo.write(baseAngleRotationDegrees);
  delay(2000);
  //baseServo.write(baseHomePosition);
  //delay(2000);
  //elbowServo.write(elbowHomePosition);
  //delay(2000);
}
void sendCoordinatesToPC()
{
// send the point data to the PC
Serial.print("<P");
Serial.print(coordinates[0]);
Serial.print(",");
Serial.print(coordinates[1]);
Serial.println(">");
}
// alternative to the readBytes function:
void getDataFromPC() 
{
  // receive data from PC and save it into inputBuffer
if(Serial.available() > 0) 
{
  char x = Serial.read();
  
  // the order of these IF clauses is significant
  if (x == endMarker)
  {
    readInProgress = false;
    char x = Serial.read();
    // the order of these I
    newDataFromPC = true;
    inputBuffer[bytesRecvd] = 0;
    coordinates[0] = inputBuffer[0];
    coordinates[1] = inputBuffer[1];
    coordinates[2] = inputBuffer[2];
  }
  if(readInProgress) 
  {
    inputBuffer[bytesRecvd] = x;
    bytesRecvd ++;
    if (bytesRecvd == buffSize) 
  {
    bytesRecvd = buffSize - 1;
  }
  }
if (x == startMarker) 
{
bytesRecvd = 0;
readInProgress = true;
}
}
}
