#include <WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

const char* ssid = "-------";
const char* password = "-------";
const char* OTA_pwd = "-------";

const int in1 = 25;    // Motor driver control IO
const int in2 = 26;
const int in3 = 27;
const int in4 = 14;
const int rightPWM = 15;   // Driver PWM IO
const int leftPWM = 13;

const int throttlePin = 34;
const int aux_throttlePin = 35;
const int steeringPin = 32;
const int throttleKnobPin = 33;

int throttlePos;
int aux_throttlePos;
int steeringPos;
int throttleKnob;

int RMotorSpeed;
String RMotorDir = "f";
int LMotorSpeed;
String LMotorDir = "f";

void setup() {
  Serial.begin(115200);                                   // Serial Connection
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);                                    // Set Wifi mode to Station (connecting to an existing wifi)
  WiFi.begin(ssid, password);
  // If connection fails, restart ESP32
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(2000);
    ESP.restart();                                        // Restart ESP32
  }

  // ------- Wifi Connected -------

  OTA();                                                  // Setup OTA
  
  Serial.println("Ready");
  // Print ESP32 IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ------- Drive Code Setup -------
  
  // Setup control pins
  pinMode(throttlePin, INPUT);
  pinMode(aux_throttlePin, INPUT);
  pinMode(steeringPin, INPUT);
  pinMode(throttleKnobPin, INPUT);
  // Setup motor driver pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  ledcSetup(0, 5000, 8);                                  // Setup channel 0 PWM - freq: 5000Hz , resolution: 8 Bits
  ledcAttachPin(rightPWM, 0);                             // Attach channel 0 PWM to right motor PWM
  ledcSetup(1, 5000, 8);                                  // Setup channel 1 PWM - freq: 5000Hz , resolution: 8 Bits
  ledcAttachPin(leftPWM, 1);                              // Attach channel 1 PWM to left motor PWM
}

// OTA setup function
void OTA() {
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  ArduinoOTA.setPassword(OTA_pwd);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");

      delay(2000);
    });

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();                                                                  // Check and handle OTA requests
  Serial.println("\n---------------------------------------\n");                        // Separate Debug output
  // Convert PPM signals to stick positions
  throttlePos = signal_conv(throttlePin, 970, 2000);
  aux_throttlePos = signal_conv(aux_throttlePin, 970, 2000);
  steeringPos = signal_conv(steeringPin, 1050, 1900);
  throttleKnob = signal_conv(throttleKnobPin, 1050, 1900);

  // Equalise Left and Right motor speed if their corresponding stick's positiona are very similar (noise/RF bias/Tx discripancies removal) 
  if ((throttlePos - aux_throttlePos == 1) || (throttlePos - aux_throttlePos == -1)) {
    aux_throttlePos = throttlePos;                                                      // If they difference between them is only by 1, then make them equal
  }

  // ----------------- RIGHT MOTOR -----------------
  // Set centre deadband
  if (throttlePos > 47 && throttlePos < 56) {
    haltMotor(in1, in2, 0);
    Serial.println("RIGHT coast");
  }
  else {                                                      // right motor not in halt region
    // Convert stick position into motor speed (PWM)
    if (throttlePos > 55) {
      RMotorSpeed = map(throttlePos, 55, 100, 25, 255);      // Convert upper range of stick positions
      RMotorDir = "f";                                       // Set direction as forwards 
    }
    else {
      RMotorSpeed = map(throttlePos, 0, 47, 255, 25);       // Convert lower range of stick positions
      RMotorDir = "r";                                      // Set direction as reverse
    }
  
    RMotorSpeed = constrain(RMotorSpeed, 0, 255);           // Constrain PWM speeds to within valid limits
    RMotorSpeed = (throttleKnob * RMotorSpeed) / 100;       // Get actual right motor PWM speed after % max speed set by speed control knob

    // If motor PWM <25, set it to 0 as PWM <25 results in too low power to turn motor
    if (RMotorSpeed < 25) {
      RMotorSpeed = 0;
    }
  
    if (RMotorDir == "f") {
      motorForwards(in1, in2, 0, RMotorSpeed);              // Run right motor forwards at calculated speed
    }
    else {
      motorReverse(in1, in2, 0, RMotorSpeed);               // Run right motor reevrse at calculated speed
    }
    Serial.print("RIGHT motor: "); Serial.print(RMotorSpeed); Serial.print(" , "); Serial.println(RMotorDir); 
  }

  // ----------------- LEFT MOTOR -----------------
  // Set centre deadband
  if (aux_throttlePos > 47 && aux_throttlePos < 56) {
    haltMotor(in3, in4, 1);
    Serial.println("LEFT coast");
  }
  else {                                                    // Left motor not in halt region
    if (aux_throttlePos > 55) {                             
      LMotorSpeed = map(aux_throttlePos, 55, 100, 25, 255); // Convert upper range of stick positions
      LMotorDir = "f";                                      // Set direction as forwards
    }
    else {
      LMotorSpeed = map(aux_throttlePos, 0, 47, 255, 25);   // Convert lower range of stick positions
      LMotorDir = "r";                                      // Set direction as reverse
    }
  
    LMotorSpeed = constrain(LMotorSpeed, 0, 255);           // Constrain PWM speeds to within valid limits
    LMotorSpeed = (throttleKnob * LMotorSpeed) / 100;       // Get actual left motor PWM speed after % max speed set by speed control knob
    
    // If motor PWM <25, set it to 0 as PWM <25 results in too low power to turn motor
    if (LMotorSpeed < 25) {
      LMotorSpeed = 0;
    }
    
    if (LMotorDir == "f") {
      motorForwards(in3, in4, 1, LMotorSpeed);              // Run left motor forwards at calculated speed
    }
    else {
      motorReverse(in3, in4, 1, LMotorSpeed);              // Run left motor reverse at calculated speed
    }
    Serial.print("LEFT motor: "); Serial.print(LMotorSpeed); Serial.print(" , "); Serial.println(LMotorDir);
  }
}

// Reads and converts PPM to stick position in %
int signal_conv(int pin, int LB, int UB) {
  int a;
  int average = 0;
  int stick_pos = 0;
  int signal_out = 0;
  
  for(a=0; a<3; a++) {
    average = average + pulseIn(pin, HIGH, 25000); // Read the pulse width of the channel 
  }
  signal_out = average / 3;                       // Average out
  stick_pos = map(signal_out, LB, UB, 0, 100);    // Convert PPM to stick position (0% to 100%)
  stick_pos = constrain(stick_pos, 0, 100);       // Constrain stick position to within valid limits
  return stick_pos;
}

// Halt Motor - Brief coast + Braking
void haltMotor(int pin1, int pin2, int PWMpin) {
  ledcWrite(PWMpin, 0);                           // Set Speed to 0
  // Coast
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  delay(50);                                     // Brief delay to reduce motor speed via coasting
  // Brake
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

// Run motor Forwards - Dependant on hardware connection of motor to motor driver
void motorForwards(int pin1, int pin2, int PWMpin, int Speed) {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  ledcWrite(PWMpin, Speed);
}

// Run Motor Reverse - Dependant on hardware connection of motor to motor driver
void motorReverse(int pin1, int pin2, int PWMpin, int Speed) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  ledcWrite(PWMpin, Speed);
}
