//Name: adaptiveTrafficLight
//Author: David Velasquez (dvelas25@eafit.edu.co)
//        Raul Mazo (rimazop@eafit.edu.co)
//Date: 3/11/2017
//Description: This program controls a traffic light that adapts to Day/Night context.
//             If the sensor detects that the system is in the Day State, it works as
//             a normal traffic light. In other case (Night State) it starts blinking
//             the red light. It is programmed as a Finite State Machine (FSM) algorithm.

//Library definitions

//I/O Pin labeling
//->Inputs
#define daySensor 0 //Light Dependant Resistor (LDR) daySensor on pin A0 (Analog)
//->Outputs
#define LR 24 //Red Light on pin 22
#define LY 23 //Yellow Light on pin 23
#define LG 22 //Green Light on pin 24

//FSM names labeling
//->Main FSM
#define EDAY 0  //State Day
#define ENIGHT 1  //State Night
//->Normal Traffic Light FSM
#define ER 0  //Red state
#define EG 1  //Green state
#define EY 2  //Yellow state
//->Blinking red light FSM
#define EROFF 0 //Red off state
#define ERON 1  //Red on state

//Constant definitions
const unsigned long LDRVAL = 300; //Constant ADC Value of LDR for changing from Day to Night (400 ADC)
const unsigned long TRG = 4000; //Constant time from red to green (4000 msecs)
const unsigned long TGY = 2000; //Constant time from green to yellow (2000 msecs)
const unsigned long TYR = 1000; //Constant time from yellow to red (1000 msecs)
const unsigned long TILT = 500;  //Constant time to tilt red light in night (500 msecs)

//Variables definition
//->FSM Vars
unsigned int mainState = EDAY;  //Variable to store current State for FSM Day/Night (Main FSM) initialized on EDAY
unsigned int trafficState = ER; //Variable to store current State for FSM Normal Traffic Light (Main FSM) initialized on ER
unsigned int blinkState = EROFF; //Variable to store current State for FSM blinking red light  initialized on EROFF
//->FSM Timing Vars
unsigned long tact = 0; //Variable to store actual time for all the timers
unsigned long tiniTraffic = 0;  //Variable to store initial time for Normal Traffic Light FSM
unsigned long trelTraffic = 0;  //Variable to store relative time for Normal Traffic Light FSM
unsigned long tiniBlink = 0;  //Variable to store relative time for Blinking red light FSM
unsigned long trelBlink = 0;  //Variable to store relative time for Blinking red light FSM
unsigned long tprev = 0;
//->Analog input vars
unsigned int sensorVal = 0; //Variable to store sensor LDR value in ADC (10 bits - Number between 0- 1023)

//Subroutines & Functions
//FSM normal traffic light
void fsmTraffic() {
  switch (trafficState) {
    case ER:
      //Physical Output state
      digitalWrite(LR, HIGH); //Turn red light on
      digitalWrite(LY, LOW);  //Turn yellow light off
      digitalWrite(LG, LOW);  //Turn green light off

      //Internal Variable state calculation
      trelTraffic = tact - tiniTraffic; //Calculate relative time for Traffic timer

      //Transition conditionals
      if (trelTraffic >= TRG) {
        trafficState = EG;  //Change to Green State
        Serial.println("trafficState: EG"); //Debug print current state
        tiniTraffic = millis(); //Assign new starting time for next state
      }
      break;
    case EG:
      //Physical Output state
      digitalWrite(LR, LOW); //Turn red light off
      digitalWrite(LY, LOW);  //Turn yellow light off
      digitalWrite(LG, HIGH);  //Turn green light on

      //Internal Variable state calculation
      trelTraffic = tact - tiniTraffic; //Calculate relative time for Traffic timer

      //Transition conditionals
      if (trelTraffic >= TGY) {
        trafficState = EY;  //Change to Yellow State
        Serial.println("trafficState: EY"); //Debug print current state
        tiniTraffic = millis(); //Assign new starting time for next state
      }
      break;
    case EY:
      //Physical Output state
      digitalWrite(LR, LOW); //Turn red light off
      digitalWrite(LY, HIGH);  //Turn yellow light on
      digitalWrite(LG, LOW);  //Turn green light off

      //Internal Variable state calculation
      trelTraffic = tact - tiniTraffic; //Calculate relative time for Traffic timer

      //Transition conditionals
      if (trelTraffic >= TYR) {
        trafficState = ER;  //Change to Red State
        Serial.println("trafficState: ER"); //Debug print current state
        tiniTraffic = millis(); //Assign new starting time for next state
      }
      break;
  }
}

//FSM normal traffic light
void fsmBlink() {
  switch (blinkState) {
    case EROFF:
      //Physical Output state
      digitalWrite(LR, HIGH); //Turn red light on
      digitalWrite(LY, LOW);  //Turn yellow light off
      digitalWrite(LG, LOW);  //Turn green light off

      //Internal Variable state calculation
      trelBlink = tact - tiniBlink; //Calculate relative time for Blinking timer

      //Transition conditionals
      if (trelBlink >= TILT) {
        blinkState = ERON;  //Change to Red on State
        Serial.println("blinkState: ERON"); //Debug print current state
        tiniBlink = millis(); //Assign new starting time for next state
      }
      break;
    case ERON:
      //Physical Output state
      digitalWrite(LR, LOW); //Turn red light off
      digitalWrite(LY, LOW);  //Turn yellow light off
      digitalWrite(LG, LOW);  //Turn green light off

      //Internal Variable state calculation
      trelBlink = tact - tiniBlink; //Calculate relative time for Blinking timer

      //Transition conditionals
      if (trelBlink >= TILT) {
        blinkState = EROFF;  //Change to Red off State
        Serial.println("blinkState: EROFF"); //Debug print current state
        tiniBlink = millis(); //Assign new starting time for next state
      }
      break;
  }
}

//Configuration
void setup() {
  //I/O configuration
  pinMode(LR, OUTPUT);  //Red light as Digital Output
  pinMode(LY, OUTPUT);  //Yellow light as Digital Output
  pinMode(LG, OUTPUT);  //Green light as Digital Output

  //Physical Output initial cleaning
  digitalWrite(LR, LOW); //Turn red light off
  digitalWrite(LY, LOW);  //Turn yellow light off
  digitalWrite(LG, LOW);  //Turn green light off

  //Communications
  Serial.begin(9600); //Start serial comms to debug with Serial Monitor PC (9600 bauds)
  Serial.println("mainState: EDAY");  //Debug print current state
  Serial.println("trafficState: ER");  //Debug print current state
  Serial.println("blinkState: EROFF");  //Debug print current state
  //Initial time cleaning
  tiniTraffic = millis();
  tiniBlink = millis();
}

//Run-time
void loop() {
  tact = millis();  //Take actual time for all FSM relative timing calculations
  //Main FSM
  switch (mainState) {
    case EDAY:
      //Physical Output state
      //->Controlled by FSM traffic
      fsmTraffic(); //Call FSM traffic light

      //Internal Variable state calculation
      sensorVal = analogRead(daySensor);  //Read analog value of LDR as ADC (10 bits- 0 to 1023 number)

      //Transition conditionals
      if (sensorVal <= LDRVAL) {
        mainState = ENIGHT;  //Change to Night State
        Serial.println("mainState: ENIGHT"); //Debug print current state
        tiniBlink = millis(); //Assign new starting time for next state
      }
      break;

    case ENIGHT:
      //Physical Output state
      //->Controlled by FSM traffic
      fsmBlink(); //Call FSM Blink

      //Internal Variable state calculation
      sensorVal = analogRead(daySensor);  //Read analog value of LDR as ADC (10 bits- 0 to 1023 number)

      //Transition conditionals
      if (sensorVal > LDRVAL) {
        mainState = EDAY;  //Change to Day State
        Serial.println("mainState: EDAY"); //Debug print current state
        tiniTraffic = millis(); //Assign new starting time for next state
      }
      break;
  }
  if(millis() - tprev >= 2000) {
    Serial.println("LDR: " + String(sensorVal));
    tprev = millis();
  }
}

