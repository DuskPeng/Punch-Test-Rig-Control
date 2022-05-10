#include <LiquidCrystal.h>
#include <Wire.h>

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
float average = 0;                // the average

int EncoderPin = A4;
int EncoderButtonPin = 22;
int state = 0;
int stateOld = 10;

int responseSize = 15;

byte EmergencyStop = 2;
byte  GreenButton = 2;
byte  RedButton = 2;
byte  BlackButton = 2;
byte  LeftDoorClosed = 2;
byte  RightDoorClosed = 2;
byte  ClampPlateLocked = 2;
byte  PunchUpperLimit = 2;
byte  PunchLowerLimit = 2;
byte  GuardClsed = 2;
byte  PressureSensor = 2;
byte  LoadCell = 2;
byte  LinearEncoder = 2;
byte  MotorSpeed = 2;

LiquidCrystal lcdSetting(32, 33, 34, 35, 36, 37);
LiquidCrystal lcdInfo(43,42,41,40,39,38);
LiquidCrystal lcdDiagnose(53, 52, 51, 50, 49, 48);

void setup() {
  // initialize serial communication with computer:
  Serial.begin(9600);
  
  Wire.begin(); // Set the LCD board as the Master board

  // pinMode(EncoderButtonPin, INPUT_PULLUP);

  lcdSetting.begin(20, 4);
  lcdSetting.print("Setting screen.");

  lcdInfo.begin(20, 4);
  lcdInfo.print("Info screen.");

  lcdDiagnose.begin(20, 4);
  lcdDiagnose.print("Diagnose screen.");

  Serial.println("LCD started.");

  delay(5000);
}

void loop() {
  
  byte response[responseSize]; // Initialize answer array
  Wire.requestFrom(9, responseSize); // Send request command to the Control board for information
  int i = 0; // Initialize the count of answer size
  while (Wire.available()) {
    // Recieve the response and record the size of response
    response[i] = Wire.read();
    i++;
  }
  state = response[14];
  EmergencyStop = response[0];
  GreenButton = response[1];
  RedButton = response[2];
  BlackButton = response[3];
  LeftDoorClosed = response[4];
  RightDoorClosed = response[5];
  ClampPlateLocked = response[6];
  PunchUpperLimit = response[7];
  PunchLowerLimit = response[8];
  GuardClsed = response[9];
  PressureSensor = response[10];
  LoadCell = response[11];
  LinearEncoder = response[12];
  MotorSpeed = response[13];
  
  Serial.print("response size: ");
  Serial.print(i);
  Serial.print(" | ");
//  for (byte j=0; j < responseSize; j++) {
//    Serial.print(response[i]);
//  }  
  Serial.println(state);
  DisplayLCDs(state);

  delay(200);
}

void DisplayLCDs (int state) {
  // Control the display content for three LCDs depending on the current state of machine
  // Read force | displacement | switchs data from board 1
  //
  refreshLCD(state);
  switch(state) {
    case 0:
        // Serial.println("Continue");
        updateForce();
        updateMotorSpeed();
        break;
    case 1:
        updateForce();
        updateMotorSpeed();
        break;
    case 2:
        updateForce();
        updateMotorSpeed();
        break;
    case 3:
        updateForce();
        updatePressure();
        break;
    case 4:
        updateForce();
        updatePressure();
        break;
    case 5:
        updateForce();
        updatePressure();
        break;
    case 6:
        updateForce();
        updatePressure();
        break;
    case 7:
        updateForce();
        updatePressure();
        break;
    case 8:
        updateForce();
        updatePressure();
        break;
    case 9:
        updateForce();
        updatePressure();
        break;
  }
}

void refreshLCD(int state) {
  bool refresh = !(stateOld == state); //check if need to refresh LCD
  if (refresh) {
    lcdSetting.clear();
    lcdDiagnose.clear();
    lcdInfo.clear();
  }
  switch(state) {
    case 0:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Standby");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Machine standby");
      break;
    case 1:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Start safe timer");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Adding pressure");
      break;
    case 2:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Starting pump");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Adding pressure");
      break;
    case 3:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Fulling accumulator");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Adding pressure");
      lcdDiagnose.setCursor(0,1);
      lcdDiagnose.print("Engage clamp");
      break;
    case 4:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Clamp hold");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Clamp");
      break;
    case 5:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Engaging punch");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Test ongoing");
      break;
    case 6:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Punch position held");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Pause");
      lcdDiagnose.setCursor(0,1);
      lcdDiagnose.print("User inspection");
      break;
    case 7:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Fulling accumulator");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Test ongoing");
      break;
    case 8:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Clamp retract");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Clamp release");
      break;
    case 9:
      NormalDisplay();
      lcdSetting.setCursor(0, 1);
      lcdSetting.print("Emergency stop");
      lcdSetting.setCursor(0, 3);
      lcdSetting.print("Emergency pressed");
      lcdDiagnose.setCursor(0,1);
      lcdDiagnose.print("Check Limit Switches");
      break;
    }
  stateOld = state;
}

void updateForce() {
    lcdInfoCl(1);
    lcdInfo.setCursor(0, 1);
    lcdInfo.print(LoadCell);
    lcdInfo.print(" N");
}

void updateMotorSpeed() {
    lcdInfoCl(2);
    lcdInfo.setCursor(0, 3);
    lcdInfo.print(MotorSpeed);
    lcdInfo.print(" %");
}

void updatePressure() {
    lcdInfoCl(2);
    lcdInfo.setCursor(0, 3);
    lcdInfo.print(PressureSensor);
}

void NormalDisplay() {
      lcdInfo.setCursor(0, 0);
      lcdInfo.print("Load: ");
      lcdInfo.setCursor(0, 2);
      lcdInfo.print("Motor Speed: ");

      lcdSetting.setCursor(0, 0);
      lcdSetting.print("Operation mode");
      lcdSetting.setCursor(0, 2);
      lcdSetting.print("State | ");
      lcdSetting.print(state);

      lcdDiagnose.setCursor(0,0);
      lcdDiagnose.print("Diagnose:");
}

void lcdSettingCl(int line) {
  lcdSetting.setCursor(0,line);
  for (int n=0; n<20; n++) {
    lcdSetting.print(" ");
  }
}

void lcdInfoCl(int line) {
  lcdInfo.setCursor(0,line);
  for (int n=0; n<20; n++) {
    lcdInfo.print(" ");
  }
}

void lcdDiagnoseCl(int line) {
  lcdDiagnose.setCursor(0,line);
  for (int n=0; n<20; n++) {
    lcdDiagnose.print(" ");
  }
}
