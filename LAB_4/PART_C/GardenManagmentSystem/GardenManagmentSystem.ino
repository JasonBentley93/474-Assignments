/**
 * @file GardenManagmentSystem.ino
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

//Setup Bluetooth(Tx/Rx)
SoftwareSerial BT(14,15);

void setup(){


}

/**
 * @defgroup group1 TASKS
 
 * @{
 */ 

/**
 * @brief Perform the specified operation on two integers
 * @return The result of the operation
 * @note Division by zero will return 0.
 */
void bluetoothIO(void *pvParameters){}


/**
 * @brief Perform the specified operation on two integers
 * @return The result of the operation
 * @note Division by zero will return 0.
 */
void displayDataOnLED(void *pvParameters){}


/**
 * @brief Perform the specified operation on two integers
 * @return The result of the operation
 * @note Division by zero will return 0.
 */
void waterControlStepperMotor(void *pvParameters){}


/**
 * @brief Perform the specified operation on two integers
 * @return The result of the operation
 * @note Division by zero will return 0.
 */
void moistureMeasure (void *pvParameters){}


/**
 * @brief Perform the specified operation on two integers
 * @return The result of the operation
 * @note Division by zero will return 0.
 */
void rtcIO(void *pvParameters){}


/**
 * @brief Perform the specified operation on two integers
 * @return The result of the operation
 * @note Division by zero will return 0.
 */
void lightMeasurement(void *pvParameters){}

/** @} */ // end of group1