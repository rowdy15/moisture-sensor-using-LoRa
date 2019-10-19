#include <LowPower.h>
#include <LoRaNow.h>

int sensorValue = 0;  // variable to store the value coming from the sensor
int oldSensorValue = 0;
int batteryValue = 0;
int oldBatteryValue = 0;

void onSleep()
{
  //Serial.println("Sleep");
  delay(5);
  byte sleepCounter;
for (sleepCounter = 7; sleepCounter > 0; sleepCounter--) // sleep for 32 seconds
{
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
}
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);

  //the above gives 60 seconds of sleep in total

  pinMode(8, OUTPUT);
  digitalWrite(8,HIGH);
  delay(150);
  sensorValue = analogRead(A1);
  //Serial.println("The moisture is: " + String(sensorValue));
  digitalWrite(8,LOW);
  pinMode(8, INPUT);

  //delay(200);



  //Serial.println(sensorValue);
  //if(sensorValue == 0) {sensorValue = 1;}
  if(sensorValue != oldSensorValue){
    Serial.println("Sending Message as the value is different from last reading");
    //LoRaNow.clear();
    LoRaNow.print(String(sensorValue)/*+"B"+String(batteryValue)*/);
    LoRaNow.send();
    LoRaNow.clear();
  } else {
    Serial.println("Not sending a message as the value is tha same as last time");
    //LoRaNow.clear();
    LoRaNow.print("s_" + String(sensorValue)/*+"B"+String(batteryValue)*/);
    LoRaNow.send();
    LoRaNow.clear();
    }
  oldSensorValue = sensorValue;
}

void setup() {
  pinMode(A1, INPUT);
  //pinMode(8, INPUT);

  
//  supposedly this may help lower current consumption .... not sure yet.
pinMode(A0,INPUT_PULLUP);
// A1 is used for moisture sensor
pinMode(A2,INPUT_PULLUP);
pinMode(A3, INPUT_PULLUP);
pinMode(A4,INPUT_PULLUP);
pinMode(A5, INPUT_PULLUP);
pinMode(A6,INPUT_PULLUP);
pinMode(A7, INPUT_PULLUP);
pinMode(0, INPUT_PULLUP);
pinMode(1, INPUT_PULLUP);
//pin 2 is used for LoRa
pinMode(3, INPUT_PULLUP);
pinMode(4, INPUT_PULLUP);
pinMode(5, INPUT_PULLUP);
pinMode(6, INPUT_PULLUP);
pinMode(7, INPUT_PULLUP);
pinMode(9, INPUT_PULLUP);

  //Serial.begin(115200);
  //Serial.println("LoRaNow Simple Node for Front Garden");

  LoRaNow.setFrequencyAU(); // Select the frequency 917.0 MHz - Used in Australia, Brazil and Chile

  LoRaNow.setPins(10, 2);

  if (!LoRaNow.begin()) {
    //Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  //LoRaNow.onMessage(onMessage);
  LoRaNow.onSleep(onSleep);
  LoRaNow.showStatus(Serial);
}

void loop() {
  LoRaNow.loop();    
}
