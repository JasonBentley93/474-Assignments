/**
 * @file TEST.ino
 * @brief A basic math library.
 * @author Jason Bentley
 * @date 8/18/2023
 * 
 * 
 * This file includes function prototypes for a basic math library. It includes basic arithmetic operations for integers and complex numbers.
 * 
 * 
 * 
 */

#include <Arduino_FreeRTOS.h>
#include "queue.h"
#include "task.h"

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <ButtonDebounce.h>
#include <AccelStepper.h>
#include <uRTCLib.h>
#include <Arduino.h>
#include <dht.h>

#define MOISTURE_UPPER 270
#define MOISTURE_LOWER 70

//setup push-button debouncer
ButtonDebounce button(29, 10);
int counter = 0;

//setup RTC module
uRTCLib rtc(0x68);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//setup DHT module
dht DHT;
#define DHT11_PIN 23

//setup LCD screen
const int rs = 50
        , e  = 51
        , d4 = 6
        , d5 = 7
        , d6 = 8
        , d7 = 9;
LiquidCrystal lcd(rs, e, d4, d5, d6, d7); 

//Setup stepper motor
#define MOTOR_TYPE 4
AccelStepper myStepper(MOTOR_TYPE, 2, 4, 3, 5);

//Setup Bluetooth
SoftwareSerial BT(14,15);


void setup(){
    Serial.begin(9600);
    while (!Serial) {;}

    URTCLIB_WIRE.begin();
    //comment out rtc.set() after setting desired date and time.
    // rtc.set(40, 20, 17, 3, 8, 8, 23);
    // rtc.set(second, minute, hour, dayOfWeek, dayOfMonth, month, year)
    // set day of week (1=Sunday, 7=Saturday)

    lcd.begin(16,2);

    //INIT STEPPER
    myStepper.setMaxSpeed(1000.0);
    myStepper.setAcceleration(50.0);
    myStepper.setSpeed(200);
    myStepper.moveTo(2038);

    //INIT BT
    BT.begin(9600);
    BT.println("Hello from Arduino");

}

void loop(){

// MEASURE LIGHT LEVEL and MOISTURE LEVEL
    lcd.clear();
    int lightLevel = analogRead(A8);
    int level = analogRead(A12);
    lcd.setCursor(0,0); 
    lcd.print("Light level: ");
    lcd.print(lightLevel);
    lcd.setCursor(0,1);
    lcd.print("Moisture: ");
    lcd.print(level);
    BT.println("Hello from Arduino");
    delay(3000);

//CHECK STATUS OF BLUETOOTH CONNECTION


// //RTC 
//   rtc.refresh();

//   Serial.print("Current Date & Time: ");
//   Serial.print(rtc.year());
//   Serial.print('/');
//   Serial.print(rtc.month());
//   Serial.print('/');
//   Serial.print(rtc.day());

//   Serial.print(" (");
//   Serial.print(daysOfTheWeek[rtc.dayOfWeek()-1]);
//    Serial.print(") ");

//   Serial.print(rtc.hour());
//   Serial.print(':');
//   Serial.print(rtc.minute());
//   Serial.print(':');
//   Serial.println(rtc.second());

//DHT and LCD
  lcd.clear();
  int chk = DHT.read11(DHT11_PIN);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");

  
  delay(3000);

  rtc.refresh();
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Date: ");
  lcd.print(rtc.month());
  lcd.print('/');
  lcd.print(rtc.day());
  lcd.print('/');
  lcd.print(rtc.year());
  lcd.setCursor(0,1);
  lcd.print("Time: ");
  lcd.print(rtc.hour());
  lcd.print(':');
  if(rtc.minute() < 10){
    lcd.print("0");
    lcd.print(rtc.minute());
  }else if(rtc.minute() >= 10){
    lcd.print(rtc.minute());
  }

  delay(3000);

// //MOVE STEPPER
//     if (myStepper.distanceToGo() == 0) 
// 		myStepper.moveTo(-myStepper.currentPosition());

// 	// Move the motor one step
//   myStepper.run();

//BUTTON DEBOUNCE
// button.update();
// if(button.state() == HIGH && counter == 0){
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("You pushed my ");
//      lcd.setCursor(0,1);
//      lcd.print("button hehe ;)");
//      counter++;
//      delay(3000);
//      lcd.clear();
// }else if(button.state() == HIGH && counter == 1){
//      lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("You did it again ");
//      lcd.setCursor(0,1);
//      lcd.print("hehe ;)");
//      counter++;
//      delay(3000);
//      lcd.clear();
// }else if(button.state() == HIGH && counter == 2){
//     lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("Oh baby, again? ");
//      lcd.setCursor(0,1);
//      lcd.print("Rawr ... <3");
//      counter=0;
//      delay(3000);
//      lcd.clear();
// }




}