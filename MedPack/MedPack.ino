/**************************************************************
 * 
 * Medicine packaging powered by Amazon DRS sketch for design 1 and 2
 * More info:
 * https://www.hackster.io/Abysmal/medicine-packaging-powered-by-amazon-drs-2dda73
 *  
 * This project is made for "Amazon DRS Developer Challenge"
 * contest on hackster.io. More info:
 * https://www.hackster.io/contests/DRS
 * 
 * author: Balázs Simon
 *
 **************************************************************/


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

#define DAY_IN_MILLIS 86400000
//#define DAY_IN_MILLIS 30000

char auth[] = "288e95fc6d0048fd9fb8a017fb400094";
/*char ssid[] = "UPC7658609";
char password[] = "simonCS18";*/
char ssid[] = "XRN3SE";
char password[] = "budainagyantal";
SimpleTimer timer;

int pins[] = {16, 5, 4, 0, 2, 14, 12, 13, 15, 1};
int pills[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

WidgetLED pillsLEDs[10] = {
  WidgetLED(V1), 
  WidgetLED(V2), 
  WidgetLED(V3), 
  WidgetLED(V4), 
  WidgetLED(V5), 
  WidgetLED(V6), 
  WidgetLED(V7), 
  WidgetLED(V8), 
  WidgetLED(V9), 
  WidgetLED(V10)
};
WidgetLED thresholdReachedLED(V14);

int test = 0;
int remainingPills = 10;
int notified = 0;
int threshold = 2;
int autoReorder = 1;
int snooze = 3000;
int totalSnoozeTime = 0;

long millisAtLastPill = 0L;

void checkPills() {
  lookForPills();
  updateWLEDs();
  pillsTaken();
}

void setup() {
  
  for (int i = 0; i < 10; i++) {
    pinMode(pins[i], INPUT);
  }
  
  Blynk.begin(auth, ssid, password);
  timer.setInterval(1000, checkPills);
  
  thresholdReachedLED.off();
}

void loop() {
  Blynk.run();
  timer.run();
}

void lookForPills(){
  int temp = 0;  
  for(int i = 0; i < 10; i++){
    pills[i] = digitalRead(pins[i]) == 0 ? 1 : 0;
    temp += pills[i];
  }
  if(remainingPills != temp || temp == 10){
    millisAtLastPill = millis();
    remainingPills = temp;
  }
}

void updateWLEDs(){
  
  for(int i = 0; i < 10; i++){
    if(pills[i]){
      pillsLEDs[i].on();
    }
    else{
      pillsLEDs[i].off();
    }
    
    if(threshold >= remainingPills){
      thresholdReachedLED.on();
      if(!notified){
        if(autoReorder){
          Blynk.notify("You're about to run out of Vitamin C, but don't worry I ordered new ones from Amazon");
        }
        else{
          Blynk.notify("Attention! DRS is disabled for this device and you're about to run out of Vitamin C");
        }
        notified = 1;
      }
    }
  }
}

void pillsTaken(){
  if(remainingPills < 10 && remainingPills > 0 && millis() - millisAtLastPill > DAY_IN_MILLIS + totalSnoozeTime){
    Blynk.notify("You forgot to take your daily Vitamin C pill");
    totalSnoozeTime += snooze;
  }

  if(millis() - millisAtLastPill < DAY_IN_MILLIS){
    totalSnoozeTime = 0;
  }
}

BLYNK_WRITE(V0)
{
  threshold = param.asInt();
}

BLYNK_WRITE(V11)
{
  autoReorder = param.asInt();
}

BLYNK_WRITE(V12)
{
  notified = 1;
  Blynk.notify("I ordered new Vitamin C from Amazon");
}

BLYNK_WRITE(V15)
{
  snooze = 60 * param.asInt() * 1000;
}
