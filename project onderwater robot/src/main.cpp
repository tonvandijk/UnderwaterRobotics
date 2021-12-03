#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

int state = 0;

bool motor_state;
bool kleppen_state; 
bool depth_state;
unsigned long delayTime;

void setup() {
  Serial.begin(9600);
  Serial.println("test");

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring! and restart!");
    while (1);
  }
  Serial.println();
}

void loop() { 
  print_depht();

  switch (state){
    case 1:
    //kleppen open
    case 2:
    //jets aan
    turn_jets(50);
    case 3:
    //delay
    case 4:
    //kleppe dicht
    case 5:
    //jets uit
    turn_jets(0);
    default:
    //nothing
  }
}





void print_depht() {
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  Serial.println();
}

void print_temperatuur(){
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
}

void turn_jets(int snelheid /*0-100*/){

}