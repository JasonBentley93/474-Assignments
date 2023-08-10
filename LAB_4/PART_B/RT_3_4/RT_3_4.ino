#include <Arduino_FreeRTOS.h>
#include <arduinoFFT.h>
#include <time.h>
#include "task.h"
#include "queue.h"


const long     samples   = 128;
const double   frequency = 5000;

double vRealRecieve[samples];
double vImag[samples];
double vRealSend[samples];

// define two tasks for Blink & AnalogRead
void TaskRT3( void *pvParameters );
void TaskRT4( void *pvParameters );

TaskHandle_t RT3, RT4;

QueueHandle_t q1 = xQueueCreate(samples, sizeof(double) * samples);
QueueHandle_t q2 = xQueueCreate(1, sizeof(TickType_t));

// the setup function runs once when you press reset or power the board
void setup() {

  for(int i = 0; i < samples; i++){
    vImag[i] = 0.0;
  }
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(19200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  } 


  xTaskCreate(
    TaskRT3
    ,  "RT3"   // A name just for humans
    ,  256  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &RT3 );

  xTaskCreate(
    TaskRT4
    ,  "RT4"
    ,  256  // Stack size
    ,  NULL
    ,  3  // Priority
    ,  &RT4 );

  vTaskStartScheduler();
}

void loop(){}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

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


