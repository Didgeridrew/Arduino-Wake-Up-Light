#include <TimeLib.h>
#include <TimeAlarms.h>   //https://github.com/PaulStoffregen/TimeAlarms
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <Bounce2.h>

#define BUTTON_PIN_1 4
#define BUTTON_PIN_2 3


//Fade Up Cycle Variables and Array--------------
int duty = 0;
int steps = 64;
int sunrisespeed = 6250; // With 2 independent LED strands, this yields a fade up from dark to full brightness in about 27minutes
int i;
int j;
int pulsepinR = 11;
int pulsepinB = 10;

int lookup[64] = {1,2,4,6,9,12,16,20,25,30,36,
42,49,56,64,72,81,90,100,110,121,132,
144,156,169,182,196,210,225,240,256,272,289,
306,324,342,361,380,400,420,441,462,484,506,
529,552,576,600,625,650,676,702,729,756,784,
812,841,870,900,930,961,992,992,992};

int button2PushCounter = 0;   // counter for the number of button presses

// Instantiate a Bounce object
Bounce debouncer1 = Bounce(); 

// Instantiate another Bounce object
Bounce debouncer2 = Bounce();

//*******************************************************************************************************
void setup(){
  pinMode(pulsepinR, OUTPUT);
  pinMode(pulsepinB, OUTPUT);

 // Setup the first button with an internal pull-up :
  pinMode(BUTTON_PIN_1,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer1.attach(BUTTON_PIN_1);
  debouncer1.interval(5); // interval in ms
  
   // Setup the second button with an internal pull-up :
  pinMode(BUTTON_PIN_2,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer2.attach(BUTTON_PIN_2);
  debouncer2.interval(5); // interval in ms
  
  //Set initial LED state to "Off"
  digitalWrite(pulsepinB, LOW);
  digitalWrite(pulsepinR, LOW);
 
  //Open Serial COM and Set time with RTC
  Serial.begin(9600);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC; syncs time every 5 minutes
    if(timeStatus() != timeSet) 
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");



  //===============================Set Alarm times for Lights On/Off (hh,mm,ss,program name)==============================
  
  Alarm.alarmRepeat(6,0,0,fadeUp);        // Turn on the light
  Alarm.alarmRepeat(9,0,0,lightOff);      // Turn off the light

  //======================================================================================================================
  
  timePrint();
//  versPrint();
}

void loop() {

  Alarm.delay(100);
  debouncer1.update();
  Alarm.delay(100);
  debouncer2.update();

  // Trigger button functions on falling edge
  if (debouncer1.fell()) {
    manualControl();
  } 
  if (debouncer2.fell()) {
    timePrint();
  }
}

void manualControl() {
  button2PushCounter++;
  Serial.print("number of button pushes:  ");
  Serial.println(button2PushCounter);
  Alarm.delay(100);
    {
    if (button2PushCounter > 2)
    button2PushCounter = 0; 
    digitalWrite(pulsepinR, LOW);
    digitalWrite(pulsepinB, LOW);
    }
    if (button2PushCounter == 1){
    analogWrite(pulsepinR, 64);
    analogWrite(pulsepinB, 64);
    Alarm.delay(100);
    }
    if (button2PushCounter == 2){
    digitalWrite(pulsepinR, HIGH);
    digitalWrite(pulsepinB, HIGH);
    Alarm.delay(100);
    }
 
}

//*******************************************************************************************************
//Turn ON the light via Alarm
void fadeUp(){
  Serial.println("Alarm: - turn lights on");
  
  for (i=0; i<steps; i++)
  {
    duty = lookup[i] * 5;
    for (j=0; j<sunrisespeed; j++)
    {
      // one pulse of PWM
      digitalWrite(pulsepinR, HIGH);
      delayMicroseconds(duty);
      digitalWrite(pulsepinR, LOW);
      delayMicroseconds(5000-duty);
      }
  }
digitalWrite(pulsepinR, HIGH);
  {
  for (i=0; i<steps; i++)
    {
    duty = lookup[i] * 5;
    for (j=0; j<sunrisespeed; j++)
      {
      digitalWrite(pulsepinB, HIGH);
      delayMicroseconds(duty);
      digitalWrite(pulsepinB, LOW);
      delayMicroseconds(5000-duty);
      }
    }      
  }
digitalWrite(pulsepinB, HIGH);

(button2PushCounter == 2);

}

//*******************************************************************************************************
//Turn off the light via Alarm
void lightOff(){
  Serial.println("Alarm: - turn lights off");
  digitalWrite(pulsepinR, LOW);
  digitalWrite(pulsepinB, LOW);
  (button2PushCounter == 0);
  
}

//*******************************************************************************************************
//Check time via Serial
void timePrint(void){
    // digital clock display of the time
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(month());
    Serial.print(' ');
    Serial.print(year()); 
    Serial.println(); 
}

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}

//*******************************************************************************************************
//Announce Version
void versPrint(){
  Serial.print("WakeLight v 3.0");
}
