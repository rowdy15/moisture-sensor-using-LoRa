// DIO0 / DIO1 / DIO2 all leak lots of current when attached.  So either leave them off or maybe use pullups when not using them.
#include <LowPower.h>
#include <LoRa.h>
//#include <avr/power.h>

#define DEBUG 1

int sensorValue = 0;  // variable to store the value coming from the sensor
int oldSensorValue = 0;
int batteryValue = 0;
int oldBatteryValue = 0;


// One way of getting the battery voltate without any double or float calculations
void getBatteryVolts() {
  //http://www.gammon.com.au/adc

  // Adjust this value to your boards specific internal BG voltage x1000
  const long InternalReferenceVoltage = 1078L; // <-- change this for your ATMEga328P pin 21 AREF value (could use both 1072 or 1073 for the chip i have.

  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);

  // Let mux settle a little to get a more stable A/D conversion
  delay(50);

  // Start a conversion
  ADCSRA |= _BV( ADSC );

  // Wait for conversion to complete
  while ( ( (ADCSRA & (1 << ADSC)) != 0 ) );

  // Scale the value - calculates for straight line value
  unsigned int batV = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L;
  
  #ifdef DEBUG
  float bV = batV;  
  Serial.print("battery voltage: ");
  Serial.print((batV/100) + bV/100-(batV/100));
  Serial.println("V");
  #endif
  if (oldBatteryValue != batV){
    #ifdef DEBUG
    Serial.println("Sending Message as the battery value is different from last reading");
    #endif

    LoRa.idle();
    LoRa.beginPacket();
    LoRa.print("n");
    LoRa.print(String(batV)/*+"B"+String(batteryValue)*/);
    LoRa.endPacket(true);
  
    oldBatteryValue = batV;
  }
}

void getMoistureLevel()
{
  pinMode(8, OUTPUT);
  digitalWrite(8,HIGH);
  delay(150);
  sensorValue = analogRead(A1); 
  digitalWrite(8,LOW);
  pinMode(8, INPUT);
  #ifdef DEBUG
  Serial.println("The moisture is: " + String(sensorValue));
  #endif

  //if(sensorValue == 0) {sensorValue = 1;}
  if(sensorValue != oldSensorValue){
    #ifdef DEBUG
      Serial.println("Sending Message as the value is different from last reading");
    #endif

    LoRa.idle();
    LoRa.beginPacket();
    LoRa.print("m");
    LoRa.print(String(sensorValue)/*+"B"+String(batteryValue)*/);
    LoRa.endPacket(true);
  } 
  oldSensorValue = sensorValue;
}

void enterSleep(){
  #ifdef DEBUG
  Serial.println("Starting sleep...\n");
  delay(10);
  #endif

  LoRa.sleep();

  byte sleepCounter;
  for (sleepCounter = 225; sleepCounter > 0; sleepCounter--) // sleep for 1 hour
  {  
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
  }  

    // time to sleep
  #ifdef DEBUG
  Serial.println("Waking from sleep!");
  delay(5);
  #endif
}

void setup() {
  
//  clock_prescale_set(clock_div_2);
  
  for (byte i = 0; i <= A5; i++)
  {
  pinMode (i, INPUT);    // changed as per below
  digitalWrite (i, LOW);  //     ditto
  }
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("LoRa Node for Front Garden");
  #endif

  LoRa.setPins(10, 2);

  if (!LoRa.begin(917E6)) {
    #ifdef DEBUG
    Serial.println("LoRa init failed. Check your connections.");
    #endif
  }

  #ifdef DEBUG
  Serial.println("LoRa init succeeded.");
  #endif
}

void loop() {
  getBatteryVolts();
  getMoistureLevel();
  enterSleep();
    
}
