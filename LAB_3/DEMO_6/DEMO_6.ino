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

#define MAX_PROCESSES 5
#define READY 1
#define DEAD  2

#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

#define REVIVE_SEVSEG_TASK       &deadTaskList[1]
#define REVIVE_PLAY_SPEAKER_TASK &deadTaskList[2]
#define REVIVE_SEVSEGSMILE_TASK  &deadTaskList[3]
#define REVIVE_FLASHLED_TASK     &deadTaskList[4]

#define KILL_SEVSEG_TASK       &processList[1]
#define KILL_PLAY_SPEAKER_TASK &processList[2]
#define KILL_SEVSEGSMILE_TASK  &processList[3]
#define KILL_FLASHLED_TASK     &processList[4]


#define NOTE_TIME   1000 //100ms


int LED_timer = 0;
int LED_state = 0;

long tune_timer   = 0;
int tune_elem     = 0;
int tune_state    = 0;
long note_timer   = 0;
int times_played  = 0;

int currentTask = 0;
int sFlag       = 1;
int tFlag       = 0;

int sevSeg_Counter      = 30;
int sevSegSmile_Counter = 0;
int sevSegPattern       = 0;
int sevSeg_timer        = 0;
long timer_counter      = 0;

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
  int j = 0;

  processList[j].pid           = 0;
  processList[j].function      = task5;
  processList[j].state         = READY;
  processList[j].name          = 'e';
  processList[j].taskRunAmount = 0;
  j++;
 
  processList[j].pid           = 1;
  processList[j].function      = sevSegCounter;
  processList[j].state         = READY;
  processList[j].name          = 'a';
  processList[j].taskRunAmount = 0;
  j++;

  processList[j].pid           = 2;
  processList[j].function      = playSpeaker;
  processList[j].state         = READY;
  processList[j].name          = 'b';
  processList[j].taskRunAmount = 0;
  j++;

  processList[j].pid           = 3;
  processList[j].function      = sevSegSmile;
  processList[j].state         = READY;
  processList[j].name          = 'c';
  processList[j].taskRunAmount = 0;
  j++;

  processList[j].pid           = 4;
  processList[j].function      = flashLED;
  processList[j].state         = READY;
  processList[j].name          = 'd';
  processList[j].taskRunAmount = 0;


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

    if(tune_state == LOW && tune_timer == period_half && tune_elem != 21 && note_timer != NOTE_TIME){
        digitalWrite(TONE_PIN, HIGH);
        tune_timer = 0;
        tune_state = HIGH;
    }

     if(tune_state == HIGH && tune_timer == period_half && tune_elem != 21 && note_timer != NOTE_TIME){
        digitalWrite(TONE_PIN, LOW);
        tune_timer = 0;
        tune_state = LOW;
    }

    if(note_timer == NOTE_TIME && tune_elem != 21){
        tune_elem++;
        note_timer = 0;
        tune_timer = 0;
    }

    if(tune_elem == 21 && tune_timer == 40000){
        tune_elem = 0;
        tune_timer = 0;
        note_timer = 0;
        times_played++;
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
void task_quit(TCB* task){
    task ->state = DEAD;
  if (task->pid == 1){
    deadTaskList[1] = *task;
    memset(KILL_SEVSEG_TASK, 0, sizeof(processList[1]));
    return;
  }

  if (task -> pid == 2){
    deadTaskList[2] = *task;
    memset(KILL_PLAY_SPEAKER_TASK, 0, sizeof(processList[2]));
    return;
  }

   if (task -> pid == 3){
    deadTaskList[3] = *task;
    memset(KILL_SEVSEGSMILE_TASK, 0, sizeof(processList[3]));
    return;
  }

   if (task -> pid == 4){
    deadTaskList[4] = *task;
    memset(KILL_FLASHLED_TASK, 0, sizeof(processList[4]));
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
    processList[1] = *task;
    memset(REVIVE_SEVSEG_TASK, 0, sizeof(deadTaskList[1]));
    return;
  }

  if (task == REVIVE_PLAY_SPEAKER_TASK){
    processList[2] = *task;
    memset(REVIVE_PLAY_SPEAKER_TASK, 0, sizeof(deadTaskList[2]));
    return;
  }

   if (task == REVIVE_SEVSEGSMILE_TASK){
    processList[3] = *task;
    memset(REVIVE_SEVSEGSMILE_TASK, 0, sizeof(deadTaskList[3]));
    return;
  }

   if (task == REVIVE_FLASHLED_TASK){
    processList[4] = *task;
    memset(REVIVE_FLASHLED_TASK, 0, sizeof(deadTaskList[4]));
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
//   Serial.println(currentTask)

  if ((currentTask == MAX_PROCESSES)){
    currentTask = 0;
  }
  // otherwise sets task to new task
  // when task is in ready state, execute
  if (processList[currentTask].state != 0) {
    function_ptr(processList[currentTask].function);
  }  
  currentTask++;

  if (tFlag == 1){
    timer_counter++;
    LED_timer++;
    sevSeg_timer++;
    tune_timer++;
    note_timer++;
    sevSegSmile_Counter++;
    tFlag = 0;
  }
}

void sevSegCounter(){
    if (sevSeg_timer == 1000 && sevSeg_Counter > 0){
        sevseg.setNumber(sevSeg_Counter, 1); 
        sevSeg_Counter--; 
        sevSeg_timer = 0;
    }

    sevseg.refreshDisplay();
}

void sevSegSmile(){
    if(sevSeg_timer > 20000){
        sevSeg_timer = 0;
    }

    if (sevSegSmile_Counter == 40){
        sevSegPattern++;
        sevSeg_timer++;
        sevSegSmile_Counter = 0;
    } else if(sevSegSmile_Counter > 40){
        sevSegSmile_Counter = 0;
        sevSegPattern = 1;
        sevSeg_timer = 1;
    }


    if (sevSegPattern == 1){
    digitalWrite(23, LOW);
    digitalWrite(22, HIGH);
    digitalWrite(24, HIGH);
    digitalWrite(25, HIGH);

    digitalWrite(2, LOW);
    digitalWrite(53, LOW);
    digitalWrite(50, LOW);

    digitalWrite(53, HIGH);
    digitalWrite(49, HIGH);
    digitalWrite(52, HIGH);

    } else if (sevSegPattern == 2){
    digitalWrite(23, HIGH);
    digitalWrite(22, LOW);
    digitalWrite(24, HIGH);
    digitalWrite(25, HIGH);

    digitalWrite(53, LOW);
    digitalWrite(49, LOW);
    digitalWrite(52, LOW);

    digitalWrite(47, HIGH);
    digitalWrite(51, HIGH);
    
    }else if (sevSegPattern == 3){ 
    digitalWrite(23, HIGH);
    digitalWrite(22, HIGH);
    digitalWrite(24, LOW);
    digitalWrite(25, HIGH);


    digitalWrite(47, HIGH);
    digitalWrite(51, HIGH);

    }else if (sevSegPattern == 4){
    digitalWrite(23, HIGH);
    digitalWrite(22, HIGH);
    digitalWrite(24, HIGH);
    digitalWrite(25, LOW);

    digitalWrite(47, LOW);
    digitalWrite(51, LOW);
    
    digitalWrite(2, HIGH);
    digitalWrite(53, HIGH);
    digitalWrite(50, HIGH);

    }else if(sevSegPattern > 4){
        sevSegPattern = 0;
    }
}

void task5(){
    if (sFlag == 1){
        task_quit(KILL_SEVSEG_TASK);
        task_quit(KILL_SEVSEGSMILE_TASK);
        sFlag = 2;
    } else if (times_played == 2 && sFlag == 2){
        task_quit(KILL_PLAY_SPEAKER_TASK);
        task_start(REVIVE_SEVSEG_TASK);
        sFlag = 3;
    }else if (sevSeg_Counter < 1 && sFlag == 3 && times_played == 2){
        task_quit(KILL_SEVSEG_TASK);
        tune_elem = 0;
        tune_timer = 0;
        note_timer = 0;
        sevseg.blank();
        task_start(REVIVE_PLAY_SPEAKER_TASK);
        sFlag = 4;
    } else if (times_played == 3 && sFlag == 4){
        task_start(REVIVE_SEVSEGSMILE_TASK);
        task_quit(KILL_PLAY_SPEAKER_TASK);
        sFlag = 5;
    } else if (sevSeg_timer == 20000 && sFlag == 5){
        task_quit(KILL_SEVSEGSMILE_TASK);
        sevseg.blank();
        sFlag = 6;
    } else if (sFlag == 6){
        return;
    }
}

ISR(TIMER4_COMPA_vect) {
  tFlag = 1;
  TIFR4 |= (1 << OCF4A);
}