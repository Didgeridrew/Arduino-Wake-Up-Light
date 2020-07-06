#include <TimeLib.h>
#include <TimeAlarms.h>   //https://github.com/PaulStoffregen/TimeAlarms
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <Bounce2.h>      //https://github.com/thomasfredericks/Bounce2

#define BUTTON_PIN_1 4
#define BUTTON_PIN_2 3

//-=-=-=-=-=-=-=-=-=-=-=-=-SET ALARM TIMES HERE-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
int onHour = 5;
int onMinute = 45;
int offHour = 10;
int offMinute = 0;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int DST = 0;

//Fade Up Cycle Variables and Array--------------
int duty = 0;
int steps = 64;
int sunrisespeed = 5000; // 5000/(# of LED output pins)= fade up of ±27minutes
int sunrisespeed2 = 10000; // 5000/(# of LED output pins)= fade up of ±27minutes
int i;
int j;
int pulsepinB = 11;
int pulsepinR = 10;

int lookup[64] = {1,2,4,6,9,12,16,20,25,30,36,
42,49,56,64,72,81,90,100,110,121,132,
144,156,169,182,196,210,225,240,256,272,289,
306,324,342,361,380,400,420,441,462,484,506,
529,552,576,600,625,650,676,702,729,756,784,
812,841,870,900,930,961,972,982,992};

int manualSetting = 0;

// Instantiate a Bounce object
Bounce debouncer1 = Bounce(); 

// Instantiate another Bounce object
Bounce debouncer2 = Bounce();


//*******************************************************************************************************
void setup() {
  // Adjust the timer registers to a higher frequency to get rid of annoying hum from the LEDs:
  TCCR1B = (TCCR1B & 0b11111000) | 0x01;    
  TCCR2B = (TCCR2B & 0b11111000) | 0x01;    
  
  // Setup the output pins for the leds: 
  pinMode(pulsepinR, OUTPUT);
  pinMode(pulsepinB, OUTPUT);

  // Setup the first button with an internal pull-up :
  pinMode(BUTTON_PIN_1,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer1.attach(BUTTON_PIN_1);
  debouncer1.interval(3); // interval in ms
  
  // Setup the second button with an internal pull-up :
  pinMode(BUTTON_PIN_2,INPUT_PULLUP);
  // After setting up the button, setup the Bounce instance :
  debouncer2.attach(BUTTON_PIN_2);
  debouncer2.interval(3); // interval in ms
  
  //Set initial LED state to "Off"
  analogWrite(pulsepinR, 0);
  analogWrite(pulsepinB, 0);
 
  //Open Serial COM and Set time with RTC
  Serial.begin(9600);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC; syncs time every 5 minutes
    if(timeStatus() != timeSet) 
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");

  CheckDST();

  //Set Alarm times for Lights On/Off (hh,mm,ss,program name)
  Alarm.alarmRepeat((onHour-DST),onMinute,0, fadeUp);           //  Turn on the light
  Alarm.alarmRepeat((offHour-DST),offMinute,0, lightOff);       //  Turn off the light
  Alarm.alarmRepeat(dowSunday,2,15,0, CheckDST);               //  Check DST status

  timePrint();
  versPrint();
  Serial.end();
}

//*******************************************************************************************************
void loop() {
  Alarm.delay(100);
  debouncer1.update();
  debouncer2.update();

  // Trigger button functions on falling edge
  if (debouncer1.fell()) {
    manualNeg();
  } 
  if (debouncer2.fell()) {
    manualPos();
  }
  if (debouncer2.rose() && debouncer1.rose()) {
    fadeUp();
  }
}

//*******************************************************************************************************
void manualNeg() {
  manualSetting--;
  Alarm.delay(25);
  manualSet(); 
}
//*******************************************************************************************************
void manualPos() {
  manualSetting++;
  Alarm.delay(25);
   manualSet();
}
//*******************************************************************************************************
void manualSet() {
    if (manualSetting == 1){
    analogWrite(pulsepinR, 24);
    analogWrite(pulsepinB, 0);
    }
    if (manualSetting == 2){
    analogWrite(pulsepinR, 90);
    analogWrite(pulsepinB, 0);
    }
    if (manualSetting == 3){
    analogWrite(pulsepinR, 250);
    analogWrite(pulsepinB, 0);
    }
    if (manualSetting == 4){
    analogWrite(pulsepinR, 250);
    analogWrite(pulsepinB, 100);
    }
    if (manualSetting == 5){
    analogWrite(pulsepinR, 250);
    analogWrite(pulsepinB, 250);
    }
    if (manualSetting > 5){
    manualSetting = 0;
    }
    if (manualSetting < 0){
    manualSetting = 0;
    }
    if (manualSetting == 0){ 
    analogWrite(pulsepinR, 0);
    analogWrite(pulsepinB, 0);
    }  
//  Serial.print("Button 1 Count:  ");
//  Serial.println(button1PushCounter);
  }

//*******************************************************************************************************
//Turn ON the light via Alarm
void fadeUp(){
//  Serial.println("Alarm: - turn lights on");
  
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
duty = 0;
  {
  for (i=0; i<steps; i++)
    {
    duty = lookup[i] * 5;
    for (j=0; j<sunrisespeed2; j++)
      {
      digitalWrite(pulsepinB, HIGH);
      delayMicroseconds(duty);
      digitalWrite(pulsepinB, LOW);
      delayMicroseconds(5000-duty);
      }
    }
  }
digitalWrite(pulsepinB, HIGH);
manualSetting == 5;
}

//*******************************************************************************************************
//Turn off the light via Alarm
void lightOff(){
//  Serial.println("Alarm: - turn lights off");
  digitalWrite(pulsepinR, LOW);
  digitalWrite(pulsepinB, LOW);
  manualSetting == 0;
}

//*******************************************************************************************************
//Check time
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

//*******************************************************************************************************
// Utility function for serial clock readout: prints preceding colon and leading 0
void printDigits(int digits){
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}

//*******************************************************************************************************
//Check for Daylight Saving Time
void CheckDST(){
  
  
// ********************* Calculate offset for Sunday ********************* 
   int y = year();                // DS3231 uses two digit year (required here)
   int x = (1 + 5*y/4) % 7;       // remainder will identify which day of month
                                  // is Sunday by subtracting x from the one
                                  // or two week window.  First two weeks for March
                                  // and first week for November
 // *********** Test DST: BEGINS on 2nd Sunday of March @ 2:00 AM ********* 
   if(month() == 3 && day() == (14 - x)) {                                   
        DST = 1;                           // Daylight Savings Time is TRUE (add one hour)
       }
   if((month() == 3 && day() > (14 - x)) || (month() > 3 && month() < 11)) {
        DST = 1;
       }
// ************* Test DST: ENDS on 1st Sunday of Nov @ 2:00 AM ************       
   if(month() == 11 && day() == (7 - x)) {
        DST = 0;                            // daylight savings time is FALSE (Standard time)
       }
   if((month() == 11 && day() > (7 - x)) || month() > 11 || month() < 3) {
        DST = 0;
       }
}
//*******************************************************************************************************
//Announce Version
void versPrint(){
  Serial.print("WakeLight v3.3.11");
}
// Version notes:
// v3.3.0 Output pins switched to match wire configuration
//        Second sunrise speed added to slow doubled LED strip set
//     .1 Corrected fadeUp button2PushCounter final line
//     .2 Removed digitalWrite commands from lightOff()*
//        Rearranged serial prints for manual operation after counter change
//     .3 Changed button2PushCounter command in lightOff and fadeUp
//     .4 Defined buton push == 0 for both manual modes. Removed excessive Alarm.delays from manual modes. 
//        Added digitalWrite back to lightOff and added button count resets. Reversed intensity order for warmOnly
//     .5 Added Daylight Saving Time adjustment
//     .6 Added DST check to setup in case of unplug or reset
//     .7 Added Register change to PWM freq to get rid of audible sound at lower duty cycles
//     .8 Changed button behavior to move up and down brightness scale similarly to wake up function.
//     .9 Set DST to 0 in setup, Changed DST equation, fixed manualSetting at end of fadeup()
//     .10 Added reboot after DST change using PNP transistor to connect RST pin to ground [failed]
//     .11 Added dual-button fadeup, Modified general month rule for DST true.
