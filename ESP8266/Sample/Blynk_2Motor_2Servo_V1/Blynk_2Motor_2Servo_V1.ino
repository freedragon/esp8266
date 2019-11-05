/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  You can receive x and y coords for joystick movement within App.

  App project setup:
    Two Axis Joystick on V1 in MERGE output mode.
    MERGE mode means device will receive both x and y within 1 message

  App project setup:
    Slider widget (0...180) on V1

    !!!The DANGER of sending a signal from 0 to 180, otherwise, the servo may be damaged!!!

    You can get the X and Y coordinates for movement of the joystick widget in the app.

Project setup in the application:
Data from the two axes of the joystick are transmitted to v1 in the output mode of the merge.
Merge mode means that the device will receive as X and Y in 1 message

Project setup application:
Widget slider (0...180) at v1

!!!Danger!!! Pass signal from 0 to 180, otherwise the servo may burn!!!

To control the built-in shield driver to use the second joystick in the app Blynk configured
on the virtual pin V2
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

int dir;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "Auth Token";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "xxxxxx";
char pass[] = "xxxxxx";

Servo servoX;
Servo servoY;

//The two servos are controlled with a joystick connected to the virtual pin V1
BLYNK_WRITE(V1) {
  servoX.write(param[0].asInt());
  servoY.write(param[1].asInt());
  
}

//-------------------Control driver motor-------------------
// останов
void stop(void) {     
   analogWrite(5, 0);     
     analogWrite(4, 0); 
}  
// вперед 
void forward(void) {
     analogWrite(5, 255); analogWrite(4, 255);
     digitalWrite(0, HIGH);digitalWrite(2, HIGH); 
}  
// назад 
void backward(void) {
     analogWrite(5, 255);analogWrite(4, 255);
     digitalWrite(0, LOW);digitalWrite(2, LOW); 
}   
// влево
void left(void) {
     analogWrite(5, 255);analogWrite(4, 255);
     digitalWrite(0, LOW);digitalWrite(2, HIGH);
}   
// вправо
void right(void) {
     analogWrite(5, 255);analogWrite(4, 255);
     digitalWrite(0, HIGH); digitalWrite(2, LOW); 
}   
//--------------------------------------------------------------

//Two motors controlled with a joystick connected to the virtual pin V2
BLYNK_WRITE(V2) {
  int x = param[0].asInt();
  int y = param[1].asInt();

       if (x<250) {dir=1;}
  else if (x>774) {dir=2;}
  else if (y>774) {dir=3;}
  else if (y<250) {dir=4;}
  else if (x>=250 && x<=750 && y>=250 && y<=750) {dir=5;}
         // выбор для кнопок         
         switch (dir)   {
             case 1:  left();
                  break;             
             case 2:  right();
                 break;
             case 3:  backward();
                 break;
             case 4: forward();
                break;             
             case 5:  stop(); 
                break;         } 
  
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

  servoX.attach(15); // GPIO 15 соответствует физическому пину D8
  servoY.attach(13); // GPIO 13 соответствует физическому пину D7
  pinMode(5, OUTPUT); // motor A speed
  pinMode(4, OUTPUT); // motor B speed
  pinMode(0, OUTPUT); //  motor A direction
  pinMode(2, OUTPUT); //  motor B direction
}

void loop()
{
  Blynk.run();
}
