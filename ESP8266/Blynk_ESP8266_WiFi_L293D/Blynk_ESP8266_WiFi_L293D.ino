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
#include <BlynkSimpleEsp8266.h>

// Reference: https://www.hackster.io/andrewf1/simplest-wifi-car-using-esp8266-motorshield-37501e

/*
int PWMA=5;//Right side
int PWMB=4;//Left side
int DA=0;//Right reverse
int DB=2;//Left reverse

https://www.instructables.com/id/NodeMCU-ESP8266-Details-and-Pinout/

D1 -> 5
D2 -> 4
D3 -> 0
D4 -> 2

#define PWM_RIGHT 5 
#define PWM_LEFT  4 
  
#define DIR_RIGHT 0 
#define DIR_LEFT  2 

ESP12E Dev Kit Control Port: D1, D3 (A motor); D2, D4 (B motor)
*/
#define PWM_RIGHT D1
#define PWM_LEFT  D2

#define DIR_RIGHT D3
#define DIR_LEFT  D4

// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = "freedragon7";
const char* pass = "92715204";

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "bc2c837acf58469d8bd9a3bb3e6e88b3";
// char auth[] = "VTO8ukXGgdkKdUWslA6eb-QtNxoJ3bWl";

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

//-------------------Control driver motor-------------------
// Halt
void stopMotors(void) {
    digitalWrite(DIR_RIGHT, LOW);
    digitalWrite(DIR_LEFT, LOW); 

    digitalWrite(PWM_RIGHT, LOW);
    digitalWrite(PWM_LEFT, LOW);
}

// Forward
void forward(void) {
    digitalWrite(DIR_RIGHT, HIGH);
    digitalWrite(DIR_LEFT, HIGH); 

    digitalWrite(PWM_RIGHT, HIGH);
    digitalWrite(PWM_LEFT, HIGH); 
}

// Move backward
void backward(void) {
    digitalWrite(DIR_RIGHT, LOW);
    digitalWrite(DIR_LEFT, LOW); 

    digitalWrite(PWM_RIGHT, HIGH);
    digitalWrite(PWM_LEFT, HIGH); 
}

// Left turn
void left(void) {
    digitalWrite(DIR_RIGHT, LOW);
    digitalWrite(DIR_LEFT, HIGH);

    digitalWrite(PWM_RIGHT, HIGH);
    digitalWrite(PWM_LEFT, HIGH); 
}

// Right turn
void right(void) {
    digitalWrite(DIR_RIGHT, HIGH);
    digitalWrite(DIR_LEFT, LOW); 

    digitalWrite(PWM_RIGHT, HIGH);
    digitalWrite(PWM_LEFT, HIGH); 
}

//--------------------------------------------------------------

int dir;
int halt = 0;

BLYNK_WRITE(V0) {
  stopMotors();
}

BLYNK_WRITE(V1)
{
    int x = param[0].asInt();
    int y = param[1].asInt();

    if (x==-1 && y==-1)             //BackWard and Left
    {
        digitalWrite(PWM_RIGHT, LOW);
        digitalWrite(DIR_RIGHT, LOW);

        digitalWrite(PWM_LEFT, HIGH);
        digitalWrite(DIR_LEFT, HIGH);
    }
    else if (x==-1 && y==0)        //Left Turn
    {
        digitalWrite(PWM_RIGHT, 450);
        digitalWrite(DIR_RIGHT, HIGH);

        digitalWrite(PWM_LEFT, 450);
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x==-1 && y==1)        //Forward and Left
    {
        digitalWrite(PWM_RIGHT, LOW);
        digitalWrite(DIR_RIGHT, LOW);

        digitalWrite(PWM_LEFT, HIGH);
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x==0 && y==-1)        //BackWard
    {
        digitalWrite(PWM_RIGHT, HIGH);
        digitalWrite(DIR_RIGHT, LOW);

        digitalWrite(PWM_LEFT, HIGH); 
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x==0 && y==0)        //Stay
    {
        digitalWrite(PWM_RIGHT, LOW);
        digitalWrite(DIR_RIGHT, LOW);

        digitalWrite(PWM_LEFT, LOW);
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x==0 && y==1)        //Forward
    {
        digitalWrite(PWM_RIGHT, HIGH);
        digitalWrite(DIR_RIGHT, HIGH);

        digitalWrite(PWM_LEFT, HIGH);
        digitalWrite(DIR_LEFT, HIGH);
    }
    else if (x==1 && y==-1)        //Backward and Right
    {
        digitalWrite(PWM_RIGHT, HIGH);
        digitalWrite(DIR_RIGHT, HIGH);

        digitalWrite(PWM_LEFT, LOW);
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x==1 && y==0)        //Right turn 
    {
        digitalWrite(PWM_RIGHT, 450); 
        digitalWrite(DIR_RIGHT, LOW); 

        digitalWrite(PWM_LEFT, 450); 
        digitalWrite(DIR_LEFT, HIGH); 
    }
    else if (x==1 && y==1)        //Forward and Right 
    {
        digitalWrite(PWM_RIGHT, HIGH); 
        digitalWrite(DIR_RIGHT, LOW); 

        digitalWrite(PWM_LEFT, LOW); 
        digitalWrite(DIR_LEFT, LOW);
    }
}

void setup()
{
    // Debug console
    Serial.begin(115200);

    pinMode(PWM_RIGHT, OUTPUT); // motor A speed
    pinMode(DIR_RIGHT, OUTPUT); //  motor A direction

    pinMode(PWM_LEFT, OUTPUT); // motor B speed
    pinMode(DIR_LEFT, OUTPUT); //  motor B direction

    Blynk.begin(auth, ssid, pass);
}

void loop()
{
    Blynk.run();
}
