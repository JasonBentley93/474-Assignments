/**
 * @file RT_1.ino
 * @brief A program to flash an external LED and play the maario theme on an external speaker using freeRTOS implemented on the Arudino Mega.
 * @author Jason Bentley
 * @date 8/16/2023
 *
 * 
 * The file includes the following modules:
 * - @link group1 INITIALIZATION @endlink
 * - @link group2 TASKS @endlink
 */

#include <Arduino_FreeRTOS.h>

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

long tune_timer   = 0; //counter to store length of time which speaker has been in a particular state
int tune_elem     = 0; //counter to store the the position of the current note in the mario[] array
int tune_state    = 0; //counter to store the current state of the speaker
int note_timer    = 0; //counter to store the length of time which a note has been playing

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};


/**
 * @defgroup group1 INITIALIZATION
 * @{
 */

/**
 * @brief This function creates all tasks necessary to run this program and also intializes TIMER4
 * @param none
 * @note see @link group2 TASKS @endlink for more information on tasks
 */
void setup() {

  TCCR4A  = 0;
  TCCR4B  = (1 << WGM42) | (1 << CS41) | (1 << CS40);
  OCR4A   = 25;
  TIMSK4 |= (1 << OCIE4A);
  sei();


  xTaskCreate(
    TaskSong
    ,  "Song"   
    ,  128  
    ,  NULL
    ,  1  
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


ISR(TIMER4_COMPA_vect) {
  sFlag = 1;
  TIFR4 |= (1 << OCF4A);
}


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