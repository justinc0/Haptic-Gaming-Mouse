//______________Setup WIFI Twilio __________________
#include "twilio.hpp"

// Set these - but DON'T push them to GitHub!
static const char *ssid = "Pixel_3895";
static const char *password = "yit8ekwhjrtvme7";

// Values from Twilio (find them on the dashboard)
static const char *account_sid = "ACab292e02481b74425c7973ae736a0a34";
static const char *auth_token = "64e69600bebba562218e6249615644b5";
// Phone number should start with "+<countrycode>"
static const char *from_number = "+19125042040";

// You choose!
// Phone number should start with "+<countrycode>"
static const char *to_number = "+15712948641";
static const char *message = "Sent from my ESP32";
int clickcount = 0;
int resetcount = 0;
int delaytimer = 0;

Twilio *twilio;


//_______________Setup Stepper code _______________
//#include "Arduino.h"
#include <Stepper.h>
const float STEPS_PER_REV = 32; 
const float GEAR_RED = 64;
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
int StepsRequired;
Stepper steppermotor(STEPS_PER_REV, 13, 14, 12, 27); //8, 10, 9, 11);

int ledPin = 26; //12   // output to LED. Use any PWM-enabled pin (marked ~)
const int sensorPin = 34; //A0  // voltage input. Use any ADC pin (starts with A)
int sensorValue = 0;  // stores value from ADC
int check=0;



void setup() {
  // ___________ Connect to WIFI and send message ______________
  Serial.begin(115200);
  /*Serial.print("Connecting to WiFi network ;");
  Serial.print(ssid);
  Serial.println("'...");*/
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    //Serial.println("Connecting...");
    delay(500);
  }
  //Serial.println("Connected!");



/*
  //____________Setup Timer_________________________________
 
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);*/
  
}

void loop() {
  String response;
  if(delaytimer >= 10000){//Every 10 seconds, reset the timer
    delaytimer = 0;
    resetcount += 1;
  }
  if(resetcount >= 720){//After 2 hours, check
    if(clickcount >= resetcount*3/4){//If at least 1 click every second for 75% of a 2-hour time period, notify
      //Send Twilio Message
      twilio = new Twilio(account_sid, auth_token);
      delay(1000);
      delaytimer += 1000;
      message = "We have detected that you have been playing intensely for 2 hours. Please take a break.";
      bool success = twilio->send_message(to_number, from_number, message, response);
    }
    else if(clickcount == 0){//If there is no activity for 2 hours, power off
      //Send Twilio Message
      twilio = new Twilio(account_sid, auth_token);
      delay(1000);
      delaytimer += 1000;
      message = "No activity was detected for 2 hours. Your haptic device has been turned off.";
      bool success = twilio->send_message(to_number, from_number, message, response);
      return;
    }
  }

  
  //____________Check force to run stepper motor__________
   sensorValue = analogRead(sensorPin);   // reads the sensor
  
  
   int minValue = 0; 
   int maxValue = 1000; 
   int constrainedValue = constrain(sensorValue, minValue, maxValue);
  
    int lite = map(constrainedValue, minValue, maxValue, 0, 255);
    
    
   int rotashun = map (constrainedValue, minValue, maxValue, 1000, 300);
    if(rotashun < 1000){ 
      clickcount += 1; //add to click count

    }
    Serial.println(rotashun);
    if(rotashun<=400){ //When player presses too hard
      //Turn on LED
      digitalWrite(ledPin,HIGH);

      //Send Twilio Message
      twilio = new Twilio(account_sid, auth_token);
      delay(1000);
      delaytimer += 1000;
      message = "There is too much force being applied to the mouse, please be more gentle.";
      bool success = twilio->send_message(to_number, from_number, message, response);

    //____________Setup LED Pin as Output_____________________
    pinMode(ledPin, OUTPUT);   
    }
    else{
      digitalWrite(ledPin,LOW);
    }
    delay(300);
    delaytimer += 300;
  
   if (sensorValue>0 & (check==0)){
      StepsRequired  =  - STEPS_PER_OUT_REV / 2;   
      steppermotor.setSpeed(rotashun);  
      steppermotor.step(1);
      
   }/*
   //____________Counter code__________________
   if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
 
    //Serial.println("An interrupt as occurred. Total number: ");
    //Serial.println(totalInterruptCounter);
 
  }*/


  
}
