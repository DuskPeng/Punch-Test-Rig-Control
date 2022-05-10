#include <Wire.h>
// Defining input and output pin numbers

//Digital Inputs
int EmergencyStop = 32; // wire 81
int GreenButton = 34; // wire 83
int RedButton = 33; // wire 82
int BlackButton = 35; // wire 84
int LeftDoorClosed = 36; // wire 85
int RightDoorClosed = 37; // wire 86
int ClampPlateLocked = 38; // wire 87
int PunchUpperLimit = 39; // wire 88
int PunchLowerLimit = 40; // wire 89
int GuardClosed = 41; // wire 810


//Digital Outputs
int GreenLamp = 28; // output wire 37   relay wire 76
int YellowLamp = 29; // output wire 38   relay wire 75
int RedLamp = 22; // output wire 31   relay wire 74
int HPUValve = 25; // output wire 34   relay wire 71
int CetopValve = 24; // output wire 33   relay wire 72
int DiverterValve = 23; // output wire 32   relay wire 73
int GuardInterlock = 26; // output wire 35   relay wire 78
int HPUMotorStart = 27; // output wire 36   relay wire 77
int Alarm = 10;


//Analogue Inputs
int PressureSensor = A1;  // wire 91
int LoadCell = A2; // wire 92
int LinearEncoder = A3; // wire NOT INSTALLED
int MotorSpeedFeed = A4; // wire 94

//Analogue (PWM) Outputs
int MotorSpeed = 2; // wire 62

//Communication
int HMI_SPI = 1; // wire NOT INSTALLED

//Defining Default Values
int MaxPumpFreq = 50;
int MinPumpFreq = 10;
int StandardPumpFreq = 42; //guessed value
int state = 0;
int PrevState = 0;
int Safe = 0;
int Abort = 0;
int AnswerSize = 15;
float Timer = 0;
float Pressure = 0;
float TargetPressurePrelim = 180;
float TargetPressure = 200;
unsigned long time;
unsigned long StartTimer;
int timeStemp;
int MSpeed;
int tempTime;

bool B_HPUV = false;
bool B_CetopV = false;
bool B_DiverterV = false;

// the setup function runs once when you press reset or power the board
void setup() {
  //Initialise I2C connection as Slave board
  Wire.begin(9);
  Wire.onRequest(requestEvent);

  //Initialise Digital Inputs
//  pinMode(EmergencyStop, INPUT_PULLUP);
  pinMode(GreenButton, INPUT_PULLUP);
  pinMode(RedButton, INPUT_PULLUP);
  pinMode(BlackButton, INPUT_PULLUP);
  pinMode(LeftDoorClosed, INPUT_PULLUP);
  pinMode(RightDoorClosed, INPUT_PULLUP);
  pinMode(ClampPlateLocked, INPUT_PULLUP);
  pinMode(PunchUpperLimit, INPUT_PULLUP);
  pinMode(PunchLowerLimit, INPUT_PULLUP);
  pinMode(GuardClosed, INPUT_PULLUP);


  //Initialise Digital Outputs
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GreenLamp, OUTPUT);
  pinMode(YellowLamp, OUTPUT);
  pinMode(RedLamp, OUTPUT);
  pinMode(HPUValve, OUTPUT);
  pinMode(CetopValve, OUTPUT);
  pinMode(DiverterValve, OUTPUT);
  pinMode(GuardInterlock, OUTPUT);
  pinMode(HPUMotorStart, OUTPUT);
  pinMode(Alarm, OUTPUT);
  

  //Initialise Analogue Inputs
  pinMode(PressureSensor, INPUT);
  pinMode(LoadCell, INPUT);
  pinMode(LinearEncoder, INPUT);


  //Initialise Analogue (PWM) Outputs
  pinMode(MotorSpeed, OUTPUT);
  Serial.begin(9600);
  Serial.println("start-up");
}

// the loop function runs over and over again forever
void loop() { // This is the main section of code from which the states
//are controlled.
restart: //restart point if the user exits the run sequence
Abort = 0;
    //CheckSafe()
    CheckSafe();
    state = 10;
    switchValve(B_HPUV, HPUValve, GreenButton, 500);
    switchValve(B_CetopV, CetopValve, RedButton, 500);
    switchValve(B_DiverterV, DiverterValve, BlackButton, 500);

    delay(20);
}


void CheckSafe() {   //A function to streamline the checking of safety 
//switches in the main code. Uses the value 0 for safe and 1 for unsafe.
  Safe = 0;

  //Safe = Safe+digitalRead(GuardClosed); //NOT YET WIRED
  //Safe = Safe+1-digitalRead(ClampPlateLocked);
  //Safe = Safe+1-digitalRead(LeftDoorClosed);
  //Safe = Safe+1-digitalRead(RightDoorClosed);
  Safe = Safe+digitalRead(EmergencyStop);
  Serial.println(digitalRead(EmergencyStop));
  Serial.println(state);
  if (Safe > 0) {  //If any sensors read unsafe, the value of 'Safe' will
  //be non-zero and the Emergency Stop state will be initiated. 
    PrevState = state;
    state = 9;
    StateMachine();
    digitalWrite(Alarm, HIGH);
    delay(150);
    digitalWrite(Alarm, LOW);
    delay(150);
    digitalWrite(Alarm, HIGH);
    delay(500);
    digitalWrite(Alarm, LOW);
    while (digitalRead(BlackButton) == HIGH){
      delay(25);
    }
    digitalWrite(YellowLamp, HIGH);
    Abort = 0;
    while (Abort == 0) {
      if (PrevState == 0) {
        break;
      }
      else if (digitalRead(RedButton) == HIGH) {
        Serial.println("Abort=1");
        Abort = 1;
      }
      else if (digitalRead(GreenButton) == LOW) {
        state = PrevState;
        StateMachine();
        Abort = 2;
      }
      delay(10);
    }
  }
}


void StateMachine() {   //This function stores the various 'states' of the
//program. It's essentially a list of common output configurations that
//can be easily switched between to tidy up the main code.
  switch(state) {
    
    case 0: //Standby (safe)
      digitalWrite(GuardInterlock,LOW);
      digitalWrite(YellowLamp,HIGH);
      digitalWrite(GreenLamp,LOW);
      digitalWrite(RedLamp,LOW);
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,LOW);
      digitalWrite(DiverterValve,LOW);
      digitalWrite(HPUMotorStart,LOW);
    break;

    case 1: //Start Button Safety Timer
      digitalWrite(GuardInterlock,LOW);
      digitalWrite(YellowLamp,HIGH);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(RedLamp,LOW);
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,LOW);
      digitalWrite(DiverterValve,LOW);
      digitalWrite(HPUMotorStart,LOW);
    break;

    case 2: //Pump Standby
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,LOW);
      digitalWrite(DiverterValve,LOW);
      digitalWrite(GuardInterlock,HIGH);
      digitalWrite(HPUMotorStart,HIGH);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,LOW);
    break;

    case 3: //Clamp Engage
      digitalWrite(HPUValve,HIGH);
      digitalWrite(CetopValve,HIGH);
      digitalWrite(DiverterValve,LOW);
      digitalWrite(GuardInterlock,HIGH);
      digitalWrite(HPUMotorStart,HIGH);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,LOW);
    break;

    case 4: //Clamp Hold
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,LOW);
      digitalWrite(DiverterValve,HIGH);
      digitalWrite(GuardInterlock,HIGH);
      digitalWrite(HPUMotorStart,HIGH);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,LOW);
    break;

    case 5: //Punch Engage
      digitalWrite(HPUValve,HIGH);
      digitalWrite(CetopValve,HIGH);
      digitalWrite(DiverterValve,HIGH);
      digitalWrite(GuardInterlock,HIGH);
      digitalWrite(HPUMotorStart,HIGH);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,LOW);
    break;

    case 6: //Punch Hold
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,HIGH);
      digitalWrite(DiverterValve,HIGH);
      digitalWrite(GuardInterlock,HIGH);
      digitalWrite(HPUMotorStart,HIGH);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,LOW);
    break;

    case 7: //Punch Retract
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,LOW);
      digitalWrite(DiverterValve,HIGH);
      digitalWrite(GuardInterlock,HIGH);
      digitalWrite(HPUMotorStart,LOW);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,LOW);
    break;

    case 8: //Clamp Retract
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,LOW);
      digitalWrite(DiverterValve,LOW);
      digitalWrite(GuardInterlock,HIGH);
      digitalWrite(HPUMotorStart,LOW);
      digitalWrite(GreenLamp,HIGH);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,LOW);
    break;

    case 9: //Emergency Stop
      digitalWrite(HPUValve,LOW);
      digitalWrite(CetopValve,LOW);
      digitalWrite(DiverterValve,LOW);
      digitalWrite(GuardInterlock,LOW);
      digitalWrite(HPUMotorStart,LOW);
      digitalWrite(GreenLamp,LOW);
      digitalWrite(YellowLamp,LOW);
      digitalWrite(RedLamp,HIGH);
    break;

  }
}

void requestEvent() {
    byte response[AnswerSize];
    response[14] = state;
    response[0] = digitalRead(EmergencyStop);
    response[1] = B_HPUV;
    response[2] = B_CetopV;
    response[3] = B_DiverterV;
    response[4] = digitalRead(LeftDoorClosed);
    response[5] = digitalRead(RightDoorClosed);
    response[6] = digitalRead(ClampPlateLocked);
    response[7] = digitalRead(PunchUpperLimit);
    response[8] = digitalRead(PunchLowerLimit);
    response[9] = digitalRead(GuardClosed);
    response[10] = Pressure;
    int Load = millis()/1000;
    response[11] = Load;
    response[12] = LinearEncoder;
    response[13] = MSpeed;
    // response[15] = 255;
    Wire.write(response, sizeof(response));
}

int simulatedValue(int timeStemp, int full) {
    int value = 0;
    if ((millis()-timeStemp) <= full) {
        delay(50);
        value = (millis()-timeStemp)/full*100;
    }
    else {
        value = 100;
    }
    return value;
}

bool switchValve(bool bValve, int pinValve, int pinSwitch, float limit) {
    if (SafePress(pinSwitch, limit)) {
        bValve = !bValve;
    }

    if (bValve) {
        digitalWrite(pinValve, HIGH);
        Serial.println(pinSwitch);
        Serial.print(" On");
        } else {
        digitalWrite(pinValve, LOW);
        Serial.println(pinSwitch);
        Serial.print(" Off");
    }
  return bValve;
}

bool SafePress(int pinSwitch, float limit) {
    float Initial_timer = millis();
    float PassedTime = 0;
    while (digitalRead(pinSwitch) == HIGH) {
        PassedTime = millis() - Initial_timer;
        
    if (PassedTime >= limit) {
        return true;
      	state = 1;
        }
    }
    return false;
    state = 0;
}
