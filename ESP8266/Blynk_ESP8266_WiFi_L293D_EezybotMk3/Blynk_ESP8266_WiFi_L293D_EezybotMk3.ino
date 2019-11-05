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
#include <Servo.h>

// Reference: https://www.hackster.io/andrewf1/simplest-wifi-car-using-esp8266-motorshield-37501e

/*
int PWMA=5;//Right side
int PWMB=4;//Left side
int DA=0;//Right reverse
int DB=2;//Left reverse

https://www.instructables.com/id/NodeMCU-ESP8266-Details-and-Pinout/

D0 -> 16
D1 -> 5
D2 -> 4
D3 -> 0
D4 -> 2

ESP12E Dev Kit Control Port: D1, D3 (A motor); D2, D4 (B motor)
*/
#define PWM_ROTATE 16
#define PWM_LOWARM 5

#define PWM_UPPERARM 4
#define PWM_GRIPPER  0

// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = "freedragon7";
const char* pass = "92715204";

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "VTO8ukXGgdkKdUWslA6eb-QtNxoJ3bWl";

WiFiClient wifiClient;
Servo baseServo,
      lowerArmServo,
      upperArmServo,
      gripperServo;

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

//--------------------------------------------------------------
// Gripper

int rotationPosition = 90;
int lowerArmPosition = 90;
int upperArmPosition = 90;

BLYNK_WRITE(V0)
{
  gripperServo.write(90 + (param.asInt() * 90));
}

BLYNK_WRITE(V1)
{
  int angle = upperArmPosition + param.asInt();
  if (angle > 180)
    angle = 179;
  else if (angle < 0)
    angle = 0;

  upperArmServo.write(angle);
  upperArmPosition = angle;
  
  Serial.print("upperArmPosition = ");
  Serial.println(upperArmPosition);
}

BLYNK_WRITE(V2)
{
  int y = param[0].asInt(),
      x = param[1].asInt();

  if (x != 0)
    x /= abs(x);
  if (y != 0)
    y /= abs(y);
 
  Serial.print("x = "); Serial.print(x); Serial.print(", y = "); Serial.println(y);

  int angle = lowerArmPosition + x;
  if (angle > 180)
    angle = 179;
  else if (angle < 0)
    angle = 0;

  int rotate = rotationPosition + y;
  if (rotate > 180)
    rotate = 179;
  else if (rotate < 0)
    rotate = 0;

  upperArmServo.write(angle);
  baseServo.write(rotate);

  lowerArmPosition = angle;
  rotationPosition = rotate;

  Serial.print("lowerArmPosition = "); Serial.print(lowerArmPosition); Serial.print(", rotationPosition = "); Serial.println(rotationPosition);
}

void setup()
{
    // Debug console
    Serial.begin(115200);

    pinMode(PWM_ROTATE, OUTPUT); // motor A speed
    pinMode(PWM_UPPERARM, OUTPUT); //  motor A direction
    pinMode(PWM_LOWARM, OUTPUT); // motor B speed
    pinMode(PWM_GRIPPER, OUTPUT); //  motor B direction

    upperArmServo.attach(PWM_UPPERARM);
    lowerArmServo.attach(PWM_LOWARM);
    gripperServo.attach(PWM_GRIPPER);

    baseServo.write(rotationPosition);
    upperArmServo.write(lowerArmPosition);
    lowerArmServo.write(upperArmPosition);

    Blynk.begin(auth, ssid, pass);
}

void loop()
{
    Blynk.run();
}
