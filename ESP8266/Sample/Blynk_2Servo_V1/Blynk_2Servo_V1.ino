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

Автор: Обушенков Алексей Андреевич
       Группа в ВК https://vk.com/engineer24
       Канал на YouTube https://www.youtube.com/channel/UCih0Gcl9IEdkR8deDJCiodg
       Инженерка Engineering room
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// You should get Auth Token in the Blynk App. 
// Go to the Project Settings (nut icon). 

char auth[] = "YourAuthToken";

// Your WiFi credentials.
// Set password to "" for open networks. Пишем название и пароль от вашего Wi-FI
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

Servo servoX; // Объявляем Серву X
Servo servoY; // Объявляем Серву Y

// Получаем от Blynk по одной виртуальной переменной значения двух осей
BLYNK_WRITE(V1) {
  servoX.write(param[0].asInt()); 
  servoY.write(param[1].asInt());
  
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);

// назначаем нашим объектам servo номер пина
  servoX.attach(15); // GPIO 15 соответствует физическому пину D8
  servoY.attach(13); // GPIO 13 соответствует физическому пину D7
}

void loop()
{
  Blynk.run();
}


