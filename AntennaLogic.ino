#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <Servo.h>
#include <IRremote.h>

// comment for git and then some
// second line
// are we being monitored?

#define servoRotatePin 4
#define rotateClockwise 1
constexpr auto rotateCounterClockwise = 2;
Servo servoRotate;
const int RECV_PIN = 3;
#define interruptPin3  1

#define antennaLeftRightPin 8
#define twinkleLightsOnOff 11

IRrecv irrecv(RECV_PIN);
uint16_t command; // decoded command;
decode_results results;

int currentPosRotate = 90;
int rotateDirClockwise = false;
int rotateStep = 5;
int button = 0;
int currentButton = 0;
int sameButtonLoopCounter = 0;
volatile int state = LOW;  // relay off left connector
 volatile unsigned long milliSecondsAtInterrupt = 0;
 unsigned long milliSecondsNow = 0;
 unsigned long runIntervalInMilliseconds = 1000;
 
void setup() {
  
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Start");
  pinMode (servoRotatePin, OUTPUT);
  
  servoRotate.attach(servoRotatePin);
    servoRotate.write(90);
    delay(100);
    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(antennaLeftRightPin, OUTPUT);
    digitalWrite(LED_BUILTIN,LOW);
    digitalWrite(antennaLeftRightPin, LOW);
 //   sleepNow();
}

void loop() {

    if (IrReceiver.decode()) 
   { 
     IrReceiver.printIRResultAsCVariables(&Serial);
     command = IrReceiver.decodedIRData.command;
     Serial.println(command);
    delay(100);
       if ( command == 25 )  
       {    
         currentButton = rotateClockwise;
         digitalWrite(antennaLeftRightPin, HIGH);
       }
       if (command == 29 )
       {
        currentButton = rotateCounterClockwise;
        digitalWrite(antennaLeftRightPin, LOW);
       } 
       // channel 2, 6 & 8 position
       if ( command == 130 )
       {
          currentPosRotate = 90;
          servoRotate.write(currentPosRotate);
          currentButton = 0;
       }
       // channel 12 position
       if ( command == 131 )
       {
          currentPosRotate = 150;
          servoRotate.write(currentPosRotate);
          currentButton = 0;
       }
       // Antenna 1 
       if( command == 54 ) 
       {
        // skip button pointing right
          digitalWrite(antennaLeftRightPin, HIGH);
       }

         // Antenna 2 
       if( command == 55 ) 
       {
        // skip button pointing left
          digitalWrite(antennaLeftRightPin, LOW);
       }

       //Twinkle LIghts ON
       if( command == 48 ) {
        digitalWrite(twinkleLightsOnOff,HIGH);
       }
       // Twinkle Lights OFF
       if(command == 49 )
       {
        digitalWrite(twinkleLightsOnOff, LOW);
       }
       
    IrReceiver.resume();        // prepare to receive the next value
    if(currentButton == 0){
      delay(200);
//      sleepNow();
    }
   }  

  if(currentButton > 0) {
    Serial.print("button pressed = ");
    Serial.println(currentButton);
    button = currentButton;
    currentButton = 0;
    switch(button){
      case rotateClockwise:
      button = 0;
        currentPosRotate += 5;
        
        if(currentPosRotate > 180){
             currentPosRotate -= 5;
             currentPosRotate = 180;
        }
        Serial.print("Rotate to ");
        Serial.println(currentPosRotate);
        servoRotate.write(currentPosRotate);
        delay(100);
//        sleepNow();
        break;

      case rotateCounterClockwise:
        currentPosRotate -= 5;
        button = 0; 
        if(currentPosRotate <= 0){
             currentPosRotate += 5;
             currentPosRotate = 180;
        }
        Serial.print("Rotate to ");
        Serial.println(currentPosRotate);
        servoRotate.write(currentPosRotate);
        delay(300);
//        sleepNow();
        break;

      //default: sleepNow();
    }
  }
}

void sleepNow()
{
//print message to serial monitor to let the user know board has gone to sleep
Serial.println("going to sleep");
//delay is added to allow user to get the full message on the serial monitor before going to sleep
delay(200);

// This is where we enable the interrupt, the reason it is done here is so that if the button is pressed accidently it doesn’t interrupt the running program.  
attachInterrupt(interruptPin3, wakeupNow, RISING);

//enables the sleep mode
sleep_enable();

/* The next line is where we choose the sleep mode we want to use for this code.  
 *  There are a few options to choose from, each with their own uses.  
 *  For more information on the sleep modes, please review the Atmega8 datasheet at [http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf)

The 5 different options for sleep modes, they are listed below from least power savings to most power savings:
  SLEEP_MODE_IDLE
  SLEEP_MODE_ADC
  SLEEP_MODE_PWR_SAVE
  SLEEP_MODE_STANDBY
  SLEEP_MODE_PWR_DOWN

For this sketch, we will be using the most power savings possible so we choose SLEEP_MODE_PWR_DOWN */

//sleep mode is set here
set_sleep_mode(SLEEP_MODE_PWR_DOWN);


//This is where the device is actually put to sleep
sleep_mode();
detachInterrupt(interruptPin3);
}

void wakeupNow(){
  sleep_disable();
  irrecv.resume();
}

 

       
