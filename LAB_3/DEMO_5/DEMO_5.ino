/**
 * @file Lab3_DDS.ino
 * @date __date__
 * @brief ECE Lab 3 Assignment
 * @detail University of Washington ECE 474
 * 
 * 
 */

#include "SevSeg.h"

#define LED_PIN  7
#define TONE_PIN 10

#define MAX_PROCESSES 2
#define READY 1
#define DEAD  2

#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

#define REVIVE_SEVSEG_TASK    &deadTaskList[0]
#define REVIVE_PLAY_SPEAKER_TASK &deadTaskList[1]

#define KILL_SEVSEG_TASK    &processList[0]
#define KILL_PLAY_SPEAKER_TASK &processList[1]

#define NOTE_TIME   333 //100ms

long tune_timer   = 0;
int tune_elem     = 0;
int tune_state    = 0;
long note_timer   = 0;

int currentTask   = 0;

int sevSeg_Counter = 0;

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

SevSeg sevseg;

/**
  * Function initializes pins to output. Also initializes tasks
  */
void setup() {
  // Initialize the required pins as output
  pinMode(LED_PIN,  OUTPUT);
  pinMode(TONE_PIN, OUTPUT);


  byte numDigits = 4;   
  byte digitPins[] = {23, 22, 24, 25}; //Digits: 1,2,3,4 <--put one resistor (ex: 220 Ohms, or 330 Ohms, etc, on each digit pin)
  byte segmentPins[] = {47, 2, 52, 51, 50, 49, 53}; //Segments: A,B,C,D,E,F,G,Period
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);

  // Initialize characteristics of tasks in your processList array
  processList[0].pid           = 0;
  processList[0].function      = sevSegCounter;
  processList[0].state         = READY;
  processList[0].name          = 'a';
  processList[0].taskRunAmount = 0;

  processList[1].pid           = 1;
  processList[1].function      = playSpeaker;
  processList[1].state         = READY;
  processList[1].name          = 'b';
  processList[1].taskRunAmount = 0;
}

/**
  * Function controls the tasks running dependent on the processList and scheduler
  * runs flashExternalLED and playSpeaker
  */
void loop() {
    scheduler();
}

/**
  * Function that plays a theme from a song
  * and then sleeps for a certain amount of time, then plays the theme again
  */
void playSpeaker() { 
    int period_half = 5000/(mario[tune_elem] + 1);

    if(tune_state == LOW && tune_timer == period_half && tune_elem != 21 && note_timer != NOTE_TIME){
        digitalWrite(TONE_PIN, HIGH);
        tune_timer = 0;
        tune_state = HIGH;
    }

     else if(tune_state == HIGH && tune_timer == period_half && tune_elem != 21 && note_timer != NOTE_TIME){
        digitalWrite(TONE_PIN, LOW);
        tune_timer = 0;
        tune_state = LOW;
    }

    else if(note_timer == NOTE_TIME && tune_elem != 21){
        tune_elem++;
        note_timer = 0;
        tune_timer = 0;
    }

    else if(tune_elem == 21 && tune_timer == 40000){
        tune_elem = 0;
        tune_timer = 0;
        note_timer = 0;
    }

    if(processList[0].function == 0){
        task_start(REVIVE_SEVSEG_TASK);
    }
    task_self_quit();
    tune_timer++;
    note_timer++;

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
        deadTaskList[0] = *KILL_SEVSEG_TASK;
        memset(&processList[0], 0, sizeof(processList[0]));
        return;
    }

    if (processList[currentTask].pid == 1){
        deadTaskList[1] = *KILL_PLAY_SPEAKER_TASK;
        memset(&processList[1], 0, sizeof(processList[currentTask]));
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

  if (task == REVIVE_SEVSEG_TASK){
    processList[0] = *task;
    memset(REVIVE_SEVSEG_TASK, 0, sizeof(deadTaskList[0]));
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
  currentTask++;
  if ((currentTask == MAX_PROCESSES)){
    currentTask = 0;
  }
  // otherwise sets task to new task
  // when task is in ready state, execute
  if (processList[currentTask].state == READY) {
    function_ptr(processList[currentTask].function);
  }  
}

void sevSegCounter(){

    if (tune_elem != 21){
        sevSeg_Counter = 41;
        sevseg.setNumber(mario[tune_elem], 1);  
    } else if (tune_elem == 21){
        if (tune_timer == 0){
            sevseg.setNumber(sevSeg_Counter, 1);
        } else if (tune_timer % 1000 == 0) {
            sevSeg_Counter--;
            sevseg.setNumber(sevSeg_Counter, 1);
        }
    }
    sevseg.refreshDisplay();
    
    if(processList[1].function == 0){
        task_start(REVIVE_PLAY_SPEAKER_TASK);
    }
    task_self_quit();
}

