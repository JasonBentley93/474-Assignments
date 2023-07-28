/*
* University of Washington
* ECE 474,  7/18/23
* Jason Bentley
*
* Lab 3 - DEMO 6
*
* Acknowledgments: LAB 3 starter code from Sep Makhsous
*/

//include the SevSeg library for controlling the 7-segment display
#include "SevSeg.h"

// Define necessary variables and constants
#define LED_PIN  7
#define TONE_PIN 10

//define max size of task struct array
#define MAX_PROCESSES 5
#define READY 1
#define DEAD  2

//define the notes for the tune
#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

//define specefic elements in the deadTaskList that will hold respective tasks
#define REVIVE_SEVSEG_TASK       &deadTaskList[1]
#define REVIVE_PLAY_SPEAKER_TASK &deadTaskList[2]
#define REVIVE_SEVSEGSMILE_TASK  &deadTaskList[3]
#define REVIVE_FLASHLED_TASK     &deadTaskList[4]

//define specefic elements in the processList that will hold respective tasks
#define KILL_SEVSEG_TASK       &processList[1]
#define KILL_PLAY_SPEAKER_TASK &processList[2]
#define KILL_SEVSEGSMILE_TASK  &processList[3]
#define KILL_FLASHLED_TASK     &processList[4]

//100ms -> based on a 100us delay, the length of time each note should play
#define NOTE_TIME   1000 //100ms


int LED_timer = 0; //counter to store length of time which LED has been in a particular state
int LED_state = 0; //counter to store the current state of the LED

long tune_timer   = 0; //counter to store length of time which speaker has been in a particular state
int tune_elem     = 0; //counter to store the the position of the current note in the mario[] array
int tune_state    = 0; //counter to store the current state of the speaker
int note_timer    = 0; //counter to store the length of time which a note has been playing
int times_played  = 0; //counter to hold the number of times the tune has been played

int currentTask = 0; //an int to hold the position of the current running task in the array
int sFlag       = 1; //a flag used to store current current step of task5()
int tFlag       = 0; //a flag raised on interupt

int sevSeg_Counter      = 30; //a counter to hold the value from which sevseg display will decrement from
int sevSeg_timer        = 0;  //an internal counter used in both sevseg tasks (smile and count)

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};

uint8_t smile[4] ={0x64, 0x9, 0x9, 0x52}; //an array to hold the smile pattern for the sevseg display

//A struct that defines the template of characteristics for one task
struct TCB {
  int pid;           //integer of the process ID
  void* function;    //the function of the process
  int state;         //state of the process
  char name;         //name of the struct
  int taskRunAmount; //integer to keep track of the times it has run
} TCB_struct;        // name given to the TCB struct

static struct TCB processList[MAX_PROCESSES]; //struct array to control READY/RUNNING tasks
static struct TCB deadTaskList[MAX_PROCESSES]; //struct array to control DEAD tasks

SevSeg sevseg; //Initialize a sevseg object


void setup() {
  // Initialize the required pins as output
  pinMode(LED_PIN,  OUTPUT);
  pinMode(TONE_PIN, OUTPUT);
  
  //setup the sevseg intilization variables
  byte numDigits = 4;   
  byte digitPins[] = {23, 22, 24, 25}; //Digits: 1,2,3,4 <--put one resistor (ex: 220 Ohms, or 330 Ohms, etc, on each digit pin)
  byte segmentPins[] = {47, 2, 52, 51, 50, 49, 53}; //Segments: A,B,C,D,E,F,G,Period
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  //initilaize the sevseg diplay
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

  //Set up timer and its configurations for ISR (CTC mode, 10KHz, prescaler = 64)
  TCCR4A = 0;
  TCCR4B = (1 << WGM42) | (1 << CS41) | (1 << CS40);
  OCR4A = 25;
  TIMSK4 |= (1 << OCIE4A);
  sei();
}


void loop() {
    scheduler();
}

/**************************************************
 * void flashLED()
 *    Arduments - no arguments
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function flashes an external LED for a hardcoded amount of time.
 *    The LED will be on for 250ms and off for 750ms
 * 
 *    Acknowledgments: N/A
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


/**************************************************
 * void playSpeaker()
 * 
 *    Arduments - no arguments
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function calculates the half-period of a given frequency 
 *    in the mario[] array. It then itterates through this array
 *    and plays each note for a NOTE_TIME amount of time. After all
 *    of the notes are played there is a hardcoded 4sec pause.
 * 
 *    Acknowledgments: N/A
 */
void playSpeaker() { 
  //calculate the half period of a frequency 
  int period_half = 5000/(mario[tune_elem] + 1);
  
  //if the speaker is off
  if(tune_state == LOW && tune_timer == period_half && tune_elem != 21 && note_timer != NOTE_TIME){
    digitalWrite(TONE_PIN, HIGH);
    tune_timer = 0;
    tune_state = HIGH;
  }

  //if the speakers it on
  if(tune_state == HIGH && tune_timer == period_half && tune_elem != 21 && note_timer != NOTE_TIME){
    digitalWrite(TONE_PIN, LOW);
    tune_timer = 0;
    tune_state = LOW;
  }

  //increment to next note if current note has played for NOTE_TIME amount of time
  if(note_timer == NOTE_TIME && tune_elem != 21){
    tune_elem++;
    note_timer = 0;
    tune_timer = 0;
  }

  //if all notes have been played wait 4sec, then go back to beginning of tune[] array
  if(tune_elem == 21 && tune_timer == 40000){
    tune_elem = 0;
    tune_timer = 0;
    note_timer = 0;
    times_played++;
  }
}


 /**************************************************
 * void function_ptr(function pointer)
 * 
 *    Argument 1 - the function being pointed to
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function serves as a function pointer to any given task to execute
 * 
 *    Acknowledgments: LAB 3 starer code
 */
void function_ptr(void* function()){
  function();
}


/**************************************************
 * void task_start(TCB*)
 * 
 *    Arguments - pointer to task control block struct
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function allows a task to terminate another task
 *    A task can call this function to change the status of a READY task to DEAD,
 *    effectively "killing" the task.
 * 
 *    Acknowledgments: LAB 3 starter code
 */  
void task_quit(TCB* task){
  //change the READY task's state to DEAD
  task ->state = DEAD;
  
  //if the current task pid = 1 move the task to deadTaskList[1]
  //and replace all values in processList[1] with NULL
  if (task->pid == 1){
    deadTaskList[1] = *task;
    memset(KILL_SEVSEG_TASK, 0, sizeof(processList[1]));
    return;
  }
 
  //if the current task pid = 2 move the task to deadTaskList[2]
  //and replace all values in processList[2] with NULL
  if (task -> pid == 2){
    deadTaskList[2] = *task;
    memset(KILL_PLAY_SPEAKER_TASK, 0, sizeof(processList[2]));
    return;
  }
   
  //if the current task pid = 3 move the task to deadTaskList[3]
  //and replace all values in processList[3] with NULL
  if (task -> pid == 3){
    deadTaskList[3] = *task;
    memset(KILL_SEVSEGSMILE_TASK, 0, sizeof(processList[3]));
    return;
  }
  
  //if the current task pid = 4 move the task to deadTaskList[4]
  //and replace all values in processList[4] with NULL
  if (task -> pid == 4){
    deadTaskList[4] = *task;
    memset(KILL_FLASHLED_TASK, 0, sizeof(processList[4]));
    return;
  }
}

/**************************************************
 * void task_start(TCB*)
 * 
 *    Arguments - pointer to task control block struct
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function allows a task to start up another task
 *    A task can call this function to change the status of a DEAD task to READY,
 *    effectively "reviving" the task.
 * 
 *    Acknowledgments: LAB 3 starter code
 */   
void task_start(TCB* task) {
  //change the DEAD task's state to ready and increment taskRunAmount
  task->state = READY;
  task->taskRunAmount++;

  //if the task is sevSegCounter move the task to processList[1]
  //and replace all values in deadTaskList[2] with NULL
  if (task == REVIVE_SEVSEG_TASK){
    processList[1] = *task;
    memset(REVIVE_SEVSEG_TASK, 0, sizeof(deadTaskList[1]));
    return;
  }
  
  //if the task is playSpeaker move the task to processList[2]
  //and replace all values in deadTaskList[3] with NULL
  if (task == REVIVE_PLAY_SPEAKER_TASK){
    processList[2] = *task;
    memset(REVIVE_PLAY_SPEAKER_TASK, 0, sizeof(deadTaskList[2]));
    return;
  }

  //if the task is sevSegSmile move the task to processList[3]
  //and replace all values in deadTaskList[3] with NULL
  if (task == REVIVE_SEVSEGSMILE_TASK){
    processList[3] = *task;
    memset(REVIVE_SEVSEGSMILE_TASK, 0, sizeof(deadTaskList[3]));
    return;
  }

  //if the task is flashLED move the task to processList[4]
  //and replace all values in deadTaskList[4] with NULL
  if (task == REVIVE_FLASHLED_TASK){
    processList[4] = *task;
    memset(REVIVE_FLASHLED_TASK, 0, sizeof(deadTaskList[4]));
    return;
  }
}


/**************************************************
 * void scheduler()
 *
 *    Arguments - no arguments
 *
 *    Returns - no returns, this is a void function
 *
 *    This function allows for tasks to be processed and executed
 *    This function will be called in the loop() function. It should check the status of
 *    each task in the tcbList and decide which one to run next, updating currentTaskIndex
 *    accordingly.
 *
 *    Acknowledgments: LAB 3 starter code
 */
void scheduler(){
  // resets currentTask to beginning of processList
  if ((currentTask == MAX_PROCESSES)){
    currentTask = 0;
  }

  // otherwise sets task to new task
  // when task is in ready state, execute
  if (processList[currentTask].state != 0) {
    function_ptr(processList[currentTask].function);
  }

  //increment to the next task
  currentTask++;
  
  //on interupt increent all internal counter and reset flag
  if (tFlag == 1){
    LED_timer++;
    sevSeg_timer++;
    tune_timer++;
    note_timer++;
    tFlag = 0;
  }
}


/**************************************************
 * void sevSegCounter()
 *
 *    Argument - no arguments
 *
 *    Returns - no returns, this is a void function
 *
 *    This tasks uses the sevSeg_timer to decrement the sevSeg_Counter
 *    every 100ms, and then displays this value on the sevseg display.
 */
void sevSegCounter(){
  //decrement sevSeg_Counter every 100ms
  if (sevSeg_timer == 1000 && sevSeg_Counter > 0){
    sevseg.setNumber(sevSeg_Counter, 1); 
    sevSeg_Counter--; 
    sevSeg_timer = 0;
    }

  sevseg.refreshDisplay();

}

/**************************************************
 * void sevSegSmile()
 *
 *    Argument - no arguments
 *
 *    Returns - no returns, this is a void function
 *
 *    This task resets the sevSeg_timer to zero and uses the
 *    sevseg library to display a smile pattern on the display
 */
void sevSegSmile(){
  //reset sevSeg_timer if it is over 2 seconds
  if(sevSeg_timer > 20000){
    sevSeg_timer = 0;
  }
    sevseg.setSegments(smile);
    sevseg.refreshDisplay();
}


/**************************************************
 * void task5()
 *
 *    Argument - no arguments
 *
 *    Returns - no returns, this is a void function
 *
 *    This task controls the other tasks through a sequence of
 *    if statements, timers/counters, and the sFlag variable.
 */
void task5(){
  //initiate only the LED task and the playSpeaker task
  if (sFlag == 1){
    task_quit(KILL_SEVSEG_TASK);
    task_quit(KILL_SEVSEGSMILE_TASK);
    sFlag = 2;

  //After playing the tune two "kill" the speaker task 
  //and initiate the sevSegCounter task
  } else if (times_played == 2 && sFlag == 2){
    task_quit(KILL_PLAY_SPEAKER_TASK);
    task_start(REVIVE_SEVSEG_TASK);
    sFlag = 3;

  //After the sevSegCounter task has counted down to zero "kill"
  //the task and "revive" the playSpeaker task
  }else if (sevSeg_Counter < 1 && sFlag == 3 && times_played == 2){
    task_quit(KILL_SEVSEG_TASK);
    tune_elem = 0;
    tune_timer = 0;
    note_timer = 0;
    sevseg.blank();
    task_start(REVIVE_PLAY_SPEAKER_TASK);
    sFlag = 4;

  //After one play through of the tune "kill" the playSpeaker task
  //and 'revive' the sevSegSmile task
  } else if (times_played == 3 && sFlag == 4){
    task_start(REVIVE_SEVSEGSMILE_TASK);
    task_quit(KILL_PLAY_SPEAKER_TASK);
    sFlag = 5;

  //After two seconds 'kill' the sevSegSmile task
  } else if (sevSeg_timer == 20000 && sFlag == 5){
    task_quit(KILL_SEVSEGSMILE_TASK);
    sevseg.blank();
    sFlag = 6;

  //Do nothing forever
  } else if (sFlag == 6){
      return;
  }
}


ISR(TIMER4_COMPA_vect) {
  tFlag = 1;
  TIFR4 |= (1 << OCF4A);
}