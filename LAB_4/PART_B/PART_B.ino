/**
 * @file PART_B.ino
 * @brief A program to flash an external LED, play the mario theme, and complete FFT calcs using freeRTOS implemented on the Arudino Mega.
 * @author Jason Bentley
 * @date 8/16/2023
 * 
 * @note acknowledgments
 *  - starter code from class
 *
 * 
 * The file includes the following modules:
 * - @link group1 INITIALIZATION @endlink
 * - @link group2 TASKS @endlink
 * - @link group3 HELPER FUNCTIONS @endlink
 */


#include <Arduino_FreeRTOS.h>
#include <arduinoFFT.h>
#include <time.h>
#include "task.h"
#include "queue.h"

#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0 

#define TONE_PIN 52

#define NOTE_TIME  1000 //100ms

int sFlag = 0;
const int maxIterations = 3;
int iteration = 0;

long tune_timer   = 0; /*!<counter to store length of time which speaker has been in a particular state*/
int tune_elem     = 0; /*!<counter to store the the position of the current note in the mario[] array*/
int tune_state    = 0; /*!<counter to store the current state of the speaker*/
int note_timer    = 0; /*!<counter to store the length of time which a note has been playing*/

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};

const long     samples   = 128;
const double   frequency = 5000;

double vRealRecieve[samples];
double vImag[samples];
double vRealSend[samples];

TaskHandle_t RT3, RT4;

QueueHandle_t q1 = xQueueCreate(samples, sizeof(double) * samples);
QueueHandle_t q2 = xQueueCreate(1, sizeof(TickType_t));



/**
 * @defgroup group1 INITIALIZATION
 * @{
 */ 

/**
 * @brief This function creates all tasks necessary to run this program as well as intilizing TIMER4 interupt in oder to play the mario theme
 * @param none
 * @note see @link group2 TASKS @endlink for more information on tasks
 */
void setup() {

  for(int i = 0; i < samples; i++){
    vImag[i] = 0.0;
  }
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(19200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  } 

  TCCR4A  = 0;
  TCCR4B  = (1 << WGM42) | (1 << CS41) | (1 << CS40);
  OCR4A   = 25;
  TIMSK4 |= (1 << OCIE4A);
  sei();

  xTaskCreate(TaskBlink, "Blink", 128, NULL, 3, NULL);
  xTaskCreate(TaskSong, "Song", 128, NULL, 2, NULL);
  xTaskCreate(TaskRT3,"RT3", 256, NULL, 3, &RT3 );
  xTaskCreate(TaskRT4, "RT4", 256, NULL, 3, &RT4);


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


/**
 * @brief This task uses the interupt timer to play the mario theme song
 * @param pvParameters pointer to allocate stack space for params
 */
void TaskSong(void *pvParameters)
{
  pinMode(TONE_PIN, OUTPUT);

  while (iteration < maxIterations){
    playSpeaker();

    if (sFlag == 1){
      tune_timer++;
      note_timer++;
      sFlag = 0;
    }

  }
  vTaskDelete(NULL);
}

/**
 * @brief This task sends data to TaskRT4 and recieves computation time data from TaskRT4
 * @param pvParameters pointer to allocate stack space for params
 */
void TaskRT3(void *pvParameters){

    TickType_t totalTime = 0, cTimeRecieve = 0;

    for(int i = 0; i < 5; i++){
        Serial.println();
        Serial.print("itteration: "); Serial.println(i+1);
        
        unsigned int tick = xTaskGetTickCount() + 1;
        srand(tick);
        
        for(int i = 0; i < samples; i++){
            vRealSend[i] = (double)rand() / RAND_MAX * (100) + 100;
        }

        if(xQueueSend(q1, &vRealSend, pdMS_TO_TICKS(100)) == pdPASS){
            Serial.println("Data Sent Successfully!");
            vTaskResume(RT4);
            vTaskSuspend(RT3);
        } else{
            Serial.println("Data Sending Failed!");
        }

        if(xQueueReceive(q2, &cTimeRecieve, portMAX_DELAY) == pdTRUE){
            totalTime = totalTime + cTimeRecieve;
            Serial.println("Computation Time Recieved Successfully");
        }else{
            Serial.println("Computation Time Recieving Failure!");
        }
    }
    Serial.println();
    Serial.print("WALL CLOCK TIME ELAPSED FOR 5 FFTs: ");
    Serial.println(totalTime * 1000 / configTICK_RATE_HZ);
    vTaskDelete(RT4);
    vTaskDelete(NULL);
}


/**
 * @brief This task recieves data from TaskRT3 compeltes an FFT and then sends computation time data to TaskRT3
 * @param pvParameters pointer to allocate stack space for params
 */
void TaskRT4(void *pvParameters){

    TickType_t cTimeSend = 0, startTime = 0, endTime = 0;

    while(1){
    if(xQueueReceive(q1, &vRealRecieve, portMAX_DELAY) == pdTRUE){
        Serial.println("Data Recieved, now processing...");
        
        startTime = xTaskGetTickCount();
        
        arduinoFFT FFT = arduinoFFT(vRealRecieve, vImag, samples, frequency);
        FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(FFT_FORWARD);

        endTime = xTaskGetTickCount();
        cTimeSend = endTime - startTime;
        Serial.println("Succesfully completed FFT...");
        Serial.println("Attempting to send data...");
    }else{
        Serial.println("Data for FFT was not recieved!");
    }

    if(xQueueSend(q2, &cTimeSend, pdMS_TO_TICKS(100) == pdPASS)){
        Serial.println("Time data was sent...");
        vTaskResume(RT3);
        vTaskSuspend(RT4);
    }else{
        Serial.println("Time data failed to send!");
    }
    }

}
/** @} */ // end of group2


/**
 * @defgroup group3 HELPER FUNCTIONS
 * @{
 */ 

/**
 * @brief This function controls which notes are played and for how long
 */
void playSpeaker() {
  //calculate the half period of a frequency  
  int period_half = 5000/(mario[tune_elem] + 1);

  //if the speaker is off
  if(tune_state == LOW && tune_timer == period_half && tune_elem != 21){
    digitalWrite(TONE_PIN, HIGH);
    tune_timer = 0;
    tune_state = HIGH;
    return;
  }
  
  //if the speakers it on
  if(tune_state == HIGH && tune_timer == period_half && tune_elem != 21){
    digitalWrite(TONE_PIN, LOW);
    tune_timer = 0;
    tune_state = LOW;
    return;
  }
  
  //increment to next note if current note has played for NOTE_TIME amount of time
  if(note_timer == NOTE_TIME && tune_elem != 21){
    tune_elem++;
    note_timer = 0;
    tune_timer = 0;
    return;
  }
  
  //if all notes have been played wait 4sec, then go back to beginning of tune[] array
  if(tune_elem == 21 && tune_timer == 15000){
    tune_elem = 0;
    tune_timer = 0;
    note_timer = 0;
    iteration++;
    return;
  }
}

/**
 * @brief This interupt raises a flag on interupt
 */
ISR(TIMER4_COMPA_vect) {
  sFlag = 1;
  TIFR4 |= (1 << OCF4A);
}
/** @} */ // end of group3
