/**
 * @file Lab3_DDS.ino
 * @date __date__
 * @brief ECE Lab 3 Assignment
 * @detail University of Washington ECE 474
 * 
 * 
 */


#include <string.h>

#define LED_PIN  7
#define TONE_PIN 10

#define MAX_PROCESSES 2
#define READY 1
#define DEAD  0

#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

#define REVIVE_FLASH_LED_TASK    &deadTaskList[0]
#define REVIVE_PLAY_SPEAKER_TASK &deadTaskList[1]

#define KILL_FLASH_LED_TASK    &processList[0]
#define KILL_PLAY_SPEAKER_TASK &processList[1]

#define NOTE_TIME   1000 //100ms


int LED_timer = 0;
int LED_state = 0;

long tune_timer   = 0;
int tune_elem     = 0;
int tune_state    = 0;
long note_timer    = 0;

int sFlag = 1;

int currentTask   = 0;
int newTask       = 0;

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};

/**
  * A struct that defines the template of characteristics for one task
  * @return a struct that contains the characteristics declared within
  */
struct TCB {
  int pid;           //integer of the process ID
  void* function;    //the function of the process
  int state;         //state of the process
  char name;         //name of the struct
  int taskRunAmount; //integer to keep track of the times it has run
} TCB_struct;        // name given to the TCB struct

static struct TCB processList[MAX_PROCESSES]; //struct array to control READY/RUNNING tasks
static struct TCB deadTaskList[MAX_PROCESSES]; //struct array to control DEAD tasks

/**
  * Function initializes pins to output. Also initializes tasks
  */
void setup() {
  // Initialize the required pins as output
  pinMode(LED_PIN,  OUTPUT);
  pinMode(TONE_PIN, OUTPUT);

  // Initialize characteristics of tasks in your processList array
  processList[0].pid           = 0;
  processList[0].function      = flashLED;
  processList[0].state         = READY;
  processList[0].name          = 'a';
  processList[0].taskRunAmount = 0;

  processList[1].pid           = 1;
  processList[1].function      = playSpeaker;
  processList[1].state         = READY;
  processList[1].name          = 'b';
  processList[1].taskRunAmount = 0;


// Initialize timer for setting up a clock.
  TCCR4A = 0;
  TCCR4B = (1 << WGM42) | (1 << CS41) | (1 << CS40);
  OCR4A = 25;

  TIMSK4 |= (1 << OCIE4A);

  sei();

}

/**
  * Function controls the tasks running dependent on the processList and scheduler
  * runs flashExternalLED and playSpeaker
  */
void loop() {
    scheduler();
}

/**
  * Flashes an external LED for a given amount of time, then turns off for another amount of time
  */
void flashLED(){
    if(LED_state == LOW && LED_timer == 7500){
        digitalWrite(LED_PIN, HIGH);
        LED_state = HIGH;
        LED_timer = 0;
    }else if(LED_state == HIGH && LED_timer == 2500){
        digitalWrite(LED_PIN, LOW);
        LED_state = LOW;
        LED_timer = 0;
    }
    return;
}


/**
  * Function that plays a theme from a song
  * and then sleeps for a certain amount of time, then plays the theme again
  */
void playSpeaker() { 
    int period_half = 5000/(mario[tune_elem] + 1);

    if(tune_timer == period_half && tune_elem != 21){
        if(tune_state == LOW){
            digitalWrite(TONE_PIN, HIGH);
            tune_timer = 0;
            tune_state = HIGH;
        } else if(tune_state == HIGH){
            digitalWrite(TONE_PIN, LOW);
            tune_timer = 0;
            tune_state = LOW;
        }
    } 
    
    if(note_timer == NOTE_TIME && tune_elem != 21){
        tune_elem++;
        note_timer = 0;
        tune_timer = 0;
    } else if(tune_elem == 21 && tune_timer == 40000){
        tune_elem = 0;
        tune_timer = 0;
        note_timer = 0;
    }

}

/**
  * This function serves as a function pointer to any given task to execute
  * @param[function()] function that needs a pointer
  */   
void function_ptr(void* function()){
    function();
}

/**\
  * This function allows a task to terminate itself by manipulating its TCB\
  * A task can call this function to change its own status to DEAD and remove itself\
  * from the tcbList.\
  */ 
void task_self_quit(){
    processList[currentTask].state = DEAD;

    if (processList[currentTask].pid == 0){
        deadTaskList[0] = *KILL_FLASH_LED_TASK;
        memset(&processList[currentTask], 0, sizeof(processList[currentTask]));
        return;
    }

    if (processList[currentTask].pid == 1){
        deadTaskList[1] = *KILL_PLAY_SPEAKER_TASK;
        memset(&processList[currentTask], 0, sizeof(processList[currentTask]));
        return;
    }
}

/**\
  * This function allows a task to start up another task\
  * A task can call this function to change the status of a DEAD task to READY,\
  * effectively "reviving" the task.\
  * @param[task] pointer to task control block struct\
  */  
void task_start(TCB* task) {
  task->state = READY;
  task->taskRunAmount++;

  if (task == REVIVE_FLASH_LED_TASK){
    processList[0] = *task;
    memset(REVIVE_FLASH_LED_TASK, 0, sizeof(deadTaskList[0]));
    return;
  }

  if (task == REVIVE_PLAY_SPEAKER_TASK){
    processList[1] = *task;
    memset(REVIVE_PLAY_SPEAKER_TASK, 0, sizeof(deadTaskList[1]));
    return;
  }
}

/**\
  * This function allows for tasks to be processed and executed\
  * This function will be called in the loop() function. It should check the status of\
  * each task in the tcbList and decide which one to run next, updating currentTaskIndex\
  * accordingly.\
  */ 
void scheduler(){
  // resets currentTask to beginning of processList
  if ((currentTask == MAX_PROCESSES)){
    currentTask = 0;
  }
  // otherwise sets task to new task
  newTask = currentTask;
  // when task is in ready state, execute
  if (processList[currentTask].state != 0) {
    function_ptr(processList[currentTask].function);
  }
  // increment currentTask to traverse processList
  currentTask++;

    if (sFlag == 1){
    LED_timer++;
    tune_timer++;
    note_timer++;
    sFlag = 0;
  }
}

ISR(TIMER4_COMPA_vect) {
  sFlag = 1;
  TIFR4 |= (1 << OCF4A);
}

