/**
 * @file RT_1.ino
 * @brief A program to flash an external LED using freeRTOS implemented on the Arudino Mega.
 * @author Jason Bentley
 * @date 8/16/2023
 *
 * 
 * The file includes the following modules:
 * - @link group1 INITIALIZATION @endlink
 * - @link group2 TASKS @endlink
 */


#include <Arduino_FreeRTOS.h>
/**
 * @defgroup group1 INITIALIZATION
 * @{
 */ 

/**
 * @brief This function creates all tasks necessary to run this program
 * @param none
 * @note see @link group2 TASKS @endlink for more information on tasks
 */
void setup() {

  xTaskCreate(
    TaskBlink
    ,  "Blink" 
    ,  128 
    ,  NULL
    ,  2  
    ,  NULL );

  vTaskStartScheduler();
}

/**
 * @brief Empty loop function necessary to compile *.ino file
 * @param none
 * @note Scheduling is handled by freeRTOS scheduler
 */
void loop(){}
/** @} */ // end of group1


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
/**
 * @defgroup group2 TASKS
 * @{
 */ 

/**
 * @brief This task blinks and LED on for 100ms and off for 200ms
 * @param pvParameters pointer to allocate stack space for params
 */
void TaskBlink(void *pvParameters)
{
  pinMode(10, OUTPUT);

  for (;;)
  {
    digitalWrite(10, HIGH);
    vTaskDelay( 100 / portTICK_PERIOD_MS ); 
    digitalWrite(10, LOW); 
    vTaskDelay( 200 / portTICK_PERIOD_MS ); 
  }
}
/** @} */ // end of group2
