/*
 Name:		retrievingv2.ino
 Created:	8/12/2021 3:32:55
 Author:	tonvandijk
*/

#include <Arduino.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Servo.h>

#define MotorInterfaceType 4
AccelStepper motor_kleppen = AccelStepper(MotorInterfaceType, 8, 9, 10, 11);
Adafruit_BME280 dieptemeter;
Servo jet1;
Servo jet2;

const int jet_pin1 = 6;
const int jet_pin2 = 5;
const int reed_contact = 7;
const short infrared2 = 4;
const short infrared1 = 3;
const int lichtsensor1 = A0;
const int lichtsensor2 = A1;


float diepte = 0;
int schakelwaarde_lichtsensor = 300;
int klep_speed = 600;
int jet_speed = 100;/*90-180*/
const int stepsPerRevolution = 200;
bool jet_state;
bool kleppen_state;
bool depth_state;
unsigned long delayTime;
int water_heigt = 100;

void setup()
{
  Serial.begin(9600);
  Serial.println("*retrieving system controller*\tv3.0\tby: UnderwaterRobotics");
  pinMode(lichtsensor1, INPUT);
  pinMode(lichtsensor2, INPUT);
  pinMode(reed_contact, INPUT);
  pinMode(infrared1, INPUT);
  pinMode(infrared2, INPUT);

 
  delay(500);
  jet1.attach(jet_pin1, 1000, 2000);
  jet1.write(90);
  Serial.println("atach jet1");
  jet2.attach(jet_pin2, 1000, 2000);
  jet2.write(90);
  Serial.println("Stach jet2");
  delay(500);
  motor_kleppen.setMaxSpeed(1000);
  Serial.println("Init steppermotor...");
  motor_kleppen.setCurrentPosition(0);
  delay(200);
  Serial.println("Steppermotors has current position set to closed");
  Serial.println("Starting dephtmeter...");
  delay(500);
  if (!dieptemeter.begin(0x76))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring! and restart!");
  }
  Serial.println();
  Serial.println("*************************************************************************");
  Serial.println("Enter your command");
}

void loop()
{
  print_depht();
  String command;

  if (Serial.available())
  {
    command = Serial.readStringUntil('\n');
    Serial.println(command);
    if (command[0] == 'o' && command[1] == 'p' && command[2] == 'e' && command[3] == 'n')
    {
      turn_kleppen(60 * 360 /*helemaal dicht*/);
    }
    else if (command[0] == 'c' && command[1] == 'l' && command[2] == 'o' && command[3] == 's' && command[4] == 'e')
    {
      turn_kleppen(0 * 360 /*helemaal dicht*/);
    }
    else if (command[0] == 'j' && command[1] == 'e' && command[2] == 't' && command[3] == '=')
    {
      if (command[4] < 48 || command[4] > 49)
      {
        Serial.println("type jet=1 to turn on / jet=0 to turn of");
      }
      else
      {
        turn_jets(command[4] - 48);
        Serial.print("jets current states are: ");
        Serial.println(jet_state);
      }
    }
    else if (command[0] == 's' && command[1] == 'e' && command[2] == 't' && command[3] == 'h')
    {
      motor_kleppen.setCurrentPosition(0);
      //deze stap kan geautomatiseert worden door een reedcontact
    }
    else if (command[0] == 'c' && command[1] == 'p' && command[2] == 'l' && command[3] == 'u' && command[4] == 's')
    {
      if (command[5] == 'x')
      {
        turn_kleppen((20 / 1.25) * 360); // for 2cm
      }
      else
      {
        turn_kleppen((1 / 1.25) * 360);
      }
    }
    else if (command[0] == 'w' && command[1] == 'a' && command[2] == 't' && command[3] == 'h' && command[4] == '=')
    {
      char getal[10] = {command[5], command[6]};
      water_heigt = atoi(getal);
      Serial.print("\nCurrent water height set to: ");
      Serial.println(water_heigt);
    }
    else if (command[0] == 's' && command[1] == 't' && command[2] == 'a' && command[3] == 't' && command[4] == 'e')

    {
      setting_state();
    }
    else if (command[0] == 'j' && command[1] == 'e' && command[2] == 't' && command[3] == 's' && command[4] == '=')
    {
      char getal[10] = {command[5], command[6], command[7]};
      jet_speed = atoi(getal);
      Serial.print("\nJet_speed set to: ");
      Serial.println(jet_speed);
    }
    else
    {
      Serial.print(command);
      Serial.println(" is een ongeldig commando");
    }
  }

  if (analogRead(lichtsensor1) < schakelwaarde_lichtsensor || analogRead(lichtsensor2 < schakelwaarde_lichtsensor) || !digitalRead(infrared1) || !digitalRead(infrared2)){
    //Serial.println("One of the sensors is blocked!");
  }
}

void print_depht()
{
  Serial.print("depht = ");
  diepte = -1 * (1.02207686 * (dieptemeter.readPressure() / 100.0F) - 1010.466067);
  Serial.print(diepte);
  Serial.print(" cm\tdistance to ground = ");
  Serial.print(water_heigt + diepte);
  Serial.println(" cm\t");
}

void print_temperatuur()
{
  Serial.print("Temperature = ");
  Serial.print(dieptemeter.readTemperature());
  Serial.println(" *C");
}

void turn_jets(bool motor_switch /*0-1*/)
{
  jet_state = motor_switch;
  jet1.write(motor_switch*jet_speed);
  jet2.write(motor_switch*jet_speed);
}

void turn_kleppen(int aantal_graden)
{
  while (motor_kleppen.currentPosition() != to_steps(aantal_graden))
  {
    motor_kleppen.setSpeed(klep_speed);
    motor_kleppen.runSpeed();
  }
}

int to_steps(int hoek)
{
  return ((stepsPerRevolution / 360) * hoek);
}

void settings()
{
  Serial.println();
  Serial.println("Actions: ");
  Serial.println("to open closure system type: (open)");
  Serial.println("to close closure system type: (close)");
  Serial.println("to turn on jets type: (jet=1/0)");
  Serial.println("to measure depht type: (depht)");
  Serial.println("to measure water temperature type: (temp)");
  Serial.println("To print sensor state type: (state)");

  Serial.println("\nSettings");
  Serial.println("to reset home posistion closure system type: (seth)");
  Serial.println("to close closure system 1mm manualy type: (cmin) ");
  Serial.println("to open closure system 1mm manualy type: (cplus)");
  Serial.println("to set water height in cm type (wat=...) -> range 10-99");

  Serial.println();
}

void setting_state()
{
  printf("jetstate: %d \n", jet_state);
  printf("kleppenstate: %d \n", jet_state);
  printf("diepte water: %d \n", water_heigt);
  printf("diepte tot bodem: %d \n", diepte);
  printf("obstacle1: %d \n", digitalRead(infrared1));
  printf("obstacle2: %d \n", digitalRead(infrared2));
  printf("lichtsensor1: %d  ", analogRead(lichtsensor1));
  if (analogRead(lichtsensor1) > schakelwaarde_lichtsensor)
  {
    printf(" --> open \n ");
  }
  else
  {
    printf(" --> blocked \n");
  }
  printf("laser2: %d  ", analogRead(lichtsensor2));
  if (analogRead(lichtsensor2) > schakelwaarde_lichtsensor)
  {
    printf(" --> open \n ");
  }
  else
  {
    printf(" --> blocked \n");
  }
}