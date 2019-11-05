/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// This source code has dependency for Blynk 0.5.2 only. (Won't build with versions above it)
#ifdef  ESP8266
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#else
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#endif

#define PWM_RIGHT D1
#define PWM_LEFT  D2
#define DIR_RIGHT D3
#define DIR_LEFT  D4

// PWM Inputs
#define RC_PWM_CH1  D5
#define RC_PWM_CH2  D6
#define RC_PWM_CH3  D7
#define RC_PWM_CH4  D8

// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = "freedragon7";
const char* pass = "92715204";

#ifdef  __USE_BLYNK__
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "VTO8ukXGgdkKdUWslA6eb-QtNxoJ3bWl";
#endif  // __USE_BLYNK__
WiFiClient wifiClient;

int rcCh1,
    rcCh2,
    rcCh3,
    rcCh4;


// This function tries to connect to the cloud using TCP
bool reconnectWIFI()
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

#ifdef  __USE_BLYNK__
int dir;
int halt = 0;

BLYNK_WRITE(V1)
{
    int x = param[0].asInt();
    int y = param[1].asInt();

    moveMotor(x, y);
}
#endif  // __USE_BLYNK__

void moveMotor(int x, int y)
{
    if (x<=-1 && y<=-1)             //BackWard and Left
    {
        digitalWrite(PWM_RIGHT, LOW);
        digitalWrite(DIR_RIGHT, LOW);

        digitalWrite(PWM_LEFT, HIGH);
        digitalWrite(DIR_LEFT, HIGH);
    }
    else if (x<=-1 && y==0)        //Left Turn
    {
        digitalWrite(PWM_RIGHT, 450);
        digitalWrite(DIR_RIGHT, HIGH);

        digitalWrite(PWM_LEFT, 450);
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x<=-1 && y>=1)        //Forward and Left
    {
        digitalWrite(PWM_RIGHT, LOW);
        digitalWrite(DIR_RIGHT, LOW);

        digitalWrite(PWM_LEFT, HIGH);
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x==0 && y<=-1)        //BackWard
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
    else if (x==0 && y>=1)        //Forward
    {
        digitalWrite(PWM_RIGHT, HIGH);
        digitalWrite(DIR_RIGHT, HIGH);

        digitalWrite(PWM_LEFT, HIGH);
        digitalWrite(DIR_LEFT, HIGH);
    }
    else if (x>=1 && y<=-1)        //Backward and Right
    {
        digitalWrite(PWM_RIGHT, HIGH);
        digitalWrite(DIR_RIGHT, HIGH);

        digitalWrite(PWM_LEFT, LOW);
        digitalWrite(DIR_LEFT, LOW);
    }
    else if (x>=1 && y==0)        //Right turn 
    {
        digitalWrite(PWM_RIGHT, 450); 
        digitalWrite(DIR_RIGHT, LOW); 

        digitalWrite(PWM_LEFT, 450); 
        digitalWrite(DIR_LEFT, HIGH); 
    }
    else if (x>=1 && y>=1)        //Forward and Right 
    {
        digitalWrite(PWM_RIGHT, HIGH); 
        digitalWrite(DIR_RIGHT, LOW); 

        digitalWrite(PWM_LEFT, LOW); 
        digitalWrite(DIR_LEFT, LOW);
    }
}


// RC PWM Signal processing

const byte MAX_ISR_COUNT = 4;

byte isr_count = 0;
byte isr_pin[MAX_ISR_COUNT];

bool isr_last_state[MAX_ISR_COUNT];
bool isr_trigger_state[MAX_ISR_COUNT];

unsigned int isr_value[MAX_ISR_COUNT];
unsigned long isr_timer[MAX_ISR_COUNT];
unsigned long isr_age[MAX_ISR_COUNT];

void ISR_generic(byte isr)
{
    unsigned long now = micros();
    bool state_now = digitalRead(isr_pin[isr]);

    if (state_now != isr_last_state[isr]) {
        if (state_now == isr_trigger_state[isr]) {
            isr_timer[isr] = now;
        } else {
            isr_value[isr] = (unsigned int)(now - isr_timer[isr]);
            isr_age[isr] = now;
        }
        isr_last_state[isr] = state_now;
    }
}

#define ISR_HANDLER(CHN)\
void ISR_##CHN() {\
    ISR_generic(CHN);\
}

ISR_HANDLER(0)
ISR_HANDLER(1)
ISR_HANDLER(2)
ISR_HANDLER(3)

void setup_isr()
{
    int my_isr = isr_count++;
    unsigned long now = micros();

    isr_pin[my_isr] = D5;
    isr_last_state[my_isr] = digitalRead(isr_pin[my_isr]);
    isr_trigger_state[my_isr] = now;

    attachInterrupt(digitalPinToInterrupt(isr_pin[my_isr]), ISR_0, CHANGE);

    my_isr = isr_count++;
    isr_pin[my_isr] = D6;
    isr_last_state[my_isr] = digitalRead(isr_pin[my_isr]);
    isr_trigger_state[my_isr] = now;
    attachInterrupt(digitalPinToInterrupt(isr_pin[my_isr]), ISR_1, CHANGE);

    my_isr = isr_count++;

    isr_pin[my_isr] = D7;
    isr_last_state[my_isr] = digitalRead(isr_pin[my_isr]);
    isr_trigger_state[my_isr] = now;

    attachInterrupt(digitalPinToInterrupt(isr_pin[my_isr]), ISR_2, CHANGE);

    my_isr = isr_count++;
    isr_pin[my_isr] = D8;
    isr_last_state[my_isr] = digitalRead(isr_pin[my_isr]);
    isr_trigger_state[my_isr] = now;
    attachInterrupt(digitalPinToInterrupt(isr_pin[my_isr]), ISR_3, CHANGE);
}

unsigned int getChannelValue(int nCh)
{
    if (nCh < 0 || nCh > MAX_ISR_COUNT)
        return 0;
    return isr_value[nCh];
}

void setup()
{
    // Debug console
    Serial.begin(115200);

    pinMode(PWM_RIGHT, OUTPUT); // motor A speed
    pinMode(DIR_RIGHT, OUTPUT); //  motor A direction

    pinMode(PWM_LEFT, OUTPUT); // motor B speed
    pinMode(DIR_LEFT, OUTPUT); //  motor B direction

    pinMode(RC_PWM_CH1, INPUT);
    pinMode(RC_PWM_CH2, INPUT);
    pinMode(RC_PWM_CH3, INPUT);
    pinMode(RC_PWM_CH4, INPUT);

    setup_isr();
#ifdef  __USE_BLYNK__
    Blynk.begin(auth, ssid, pass);
#endif  // __USE_BLYNK__
}

int prcCh1 = 0, prcCh2 = 0, prcCh3 = 0, prcCh4 = 0;

#define UPDATE_CH_VAR(nV,oV)  if (nV > 6 || nV < -6) nV = oV; else oV = nV

void loop()
{
    rcCh1 = getChannelValue(0);
    rcCh2 = getChannelValue(1);
    rcCh3 = getChannelValue(2);
    rcCh4 = getChannelValue(3);

#ifdef  __PRINT_CHANNELS__
    Serial.print("RAW: C#1:"); Serial.print(rcCh1);
    Serial.print(", C#2:"); Serial.print( rcCh2 );
    Serial.print(", C#3:"); Serial.print( rcCh3 );
    Serial.print(", C#4:"); Serial.print( rcCh4 );
#endif  // __PRINT_CHANNELS__

    rcCh1 = map(rcCh1, 1050,1800, -5, 5);
    rcCh2 = map(rcCh2, 1050,1800, -5, 5);
    rcCh3 = map(rcCh3, 1050,1800, -5, 5);
    rcCh4 = map(rcCh4, 1050,1800, -5, 5);

#ifdef  __PRINT_CHANNELS__
    Serial.print("\tMAPPED: C#1:"); Serial.print(rcCh1);
    Serial.print(", C#2:"); Serial.print( rcCh2 );
    Serial.print(", C#3:"); Serial.print( rcCh3 );
    Serial.print(", C#4:"); Serial.print( rcCh4 );
    Serial.print("\r");
    // 
    Serial.print("                                                                                                                                                                              ");
#endif  // __PRINT_CHANNELS__

    // Rule out the possible out-of-range values for each channel
    UPDATE_CH_VAR(rcCh1, prcCh1);
    UPDATE_CH_VAR(rcCh2, prcCh2);
    UPDATE_CH_VAR(rcCh3, prcCh3);
    UPDATE_CH_VAR(rcCh4, prcCh4);

    /**
    Serial.print("C#1:"); Serial.print(rcCh1);
    Serial.print(", C#2:"); Serial.print( rcCh2 );
    Serial.print(", C#3:"); Serial.print( rcCh3 );
    Serial.print(", C#4:"); Serial.print( rcCh4 );
    Serial.println(); //make some room
    */

    moveMotor(rcCh2 / 5, rcCh1 / 5);   // Move Tank based on the controller's stick
#ifdef  __USE_BLYNK__
    Blynk.run();
#endif  // __USE_BLYNK__
}
