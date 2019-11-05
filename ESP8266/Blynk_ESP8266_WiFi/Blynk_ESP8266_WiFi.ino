/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.
 *************************************************************
  This sketch shows how to access WiFiClient directly in Blynk

  1. This gives you full control of the connection process.
  2. Shows a sensible way of integrating other connectivity hardware,
     that was not supported by Blynk out-of-the-box.

  NOTE: This requires ESP8266 support package:
       https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
    in the Tools -> Board menu!

 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleStream.h>

// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = "freedragon7";
const char* pass = "92715204";

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "bc2c837acf58469d8bd9a3bb3e6e88b3";

WiFiClient wifiClient;

// This function tries to connect to the cloud using TCP
bool connectBlynk()
{
    wifiClient.stop();
    return wifiClient.connect(BLYNK_DEFAULT_DOMAIN, BLYNK_DEFAULT_PORT);
}

// This function tries to connect to your WiFi network
void connectWiFi()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    if (pass && strlen(pass)) {
        WiFi.begin((char*)ssid, (char*)pass);
    } else {
        WiFi.begin((char*)ssid);
    }
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

#define DIR_RIGHT 0
#define DIR_LEFT  2
#define PWM_RIGHT 5
#define PWM_LEFT  4

//-------------------Control driver motor-------------------
// Halt
void stop(void) {     
    analogWrite(PWM_RIGHT, 0);     
    analogWrite(PWM_LEFT, 0); 
}

// Forward
void forward(void) {
    analogWrite(PWM_RIGHT, 255);
    analogWrite(PWM_LEFT, 255);

    digitalWrite(DIR_RIGHT, HIGH);
    digitalWrite(DIR_LEFT, HIGH); 
}

// Move backward
void backward(void) {
    analogWrite(PWM_RIGHT, 255);
    analogWrite(PWM_LEFT, 255);

    digitalWrite(DIR_RIGHT, LOW);
    digitalWrite(DIR_LEFT, LOW); 
}

// Left turn
void left(void) {
    analogWrite(PWM_RIGHT, 255);
    analogWrite(PWM_LEFT, 255);

    digitalWrite(DIR_RIGHT, LOW);
    digitalWrite(DIR_LEFT, HIGH);
}

// Right turn
void right(void) {
    analogWrite(PWM_RIGHT, 255);
    analogWrite(PWM_LEFT, 255);

    digitalWrite(DIR_RIGHT, HIGH);
    digitalWrite(DIR_LEFT, LOW); 
}
//--------------------------------------------------------------

int dir;

//Two motors controlled with a joystick connected to the virtual pin V1
BLYNK_WRITE(V0) {
    int x = param[0].asInt();
    int y = param[1].asInt();

    if (x<250) {dir=1;}
    else if (x>774) {dir=2;}
    else if (y>774) {dir=3;}
    else if (y<250) {dir=4;}
    else if (x>=250 && x<=750 && y>=250 && y<=750) {dir=5;}
  
    // Call actual montor control function
    switch (dir)
    {
    case 1:
        left();
        break;
    case 2:
        right();
        break;
    case 3:
        backward();
        break;
    case 4:
        forward();
        break;
    case 5:
          stop();
          break;
    }
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  connectWiFi();

  connectBlynk();

  Blynk.begin(wifiClient, auth);
  pinMode(PWM_RIGHT, OUTPUT); // motor A speed
  pinMode(PWM_LEFT, OUTPUT); // motor B speed
  pinMode(DIR_RIGHT, OUTPUT); //  motor A direction
  pinMode(DIR_LEFT, OUTPUT); //  motor B direction
}

void loop()
{
  // Reconnect WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    return;
  }

  // Reconnect to Blynk Cloud
  if (!wifiClient.connected()) {
    connectBlynk();
    return;
  }

  Blynk.run();
}
