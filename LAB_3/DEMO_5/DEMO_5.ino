/*
* University of Washington
* ECE 474,  7/18/23
* Jason Bentley
*
* Lab 3 - DEMO 5
*
* Acknowledgments: LAB 3 starter code from Sep Makhsous
*/

//include the SevSeg library for controlling the 7-segment display
#include "SevSeg.h"

// Define necessary variables and constants
#define LED_PIN  7
#define TONE_PIN 10

//define max size of task struct array
#define MAX_PROCESSES 2

//define the possible states a task can have
#define READY 1
#define DEAD  2

//define the notes for the tune
#define NOTE_E 659
#define NOTE_C 523
#define NOTE_G 784
#define NOTE_g 392
#define NOTE_R 0

//define specefic elements in the deadTaskList that will hold respective tasks
#define REVIVE_SEVSEG_TASK    &deadTaskList[0]
#define REVIVE_PLAY_SPEAKER_TASK &deadTaskList[1]

//define specefic elements in the processList that will hold respective tasks
#define KILL_SEVSEG_TASK    &processList[0]
#define KILL_PLAY_SPEAKER_TASK &processList[1]

//100ms -> based on experimentation, the length of time each note should play
#define NOTE_TIME   333 //100ms

long tune_timer   = 0; //counter to store length of time which speaker has been in a particular state
int tune_elem     = 0; //counter to store the the position of the current note in the mario[] array
int tune_state    = 0; //counter to store the current state of the speaker
int note_timer    = 0; //counter to store the length of time which a note has been playing

int currentTask   = 0; //an int to hold the position of the current running task in the array

int sevSeg_Counter = 0; //a counter to hold the value from which sevseg display will decrement from
                        //value is set in sevseg task

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g,
              NOTE_R};


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


void loop() {
    scheduler();
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
  else if(tune_state == HIGH && tune_timer == period_half && tune_elem != 21 && note_timer != NOTE_TIME){
    digitalWrite(TONE_PIN, LOW);
    tune_timer = 0;
    tune_state = LOW;
  }

  //increment to next note if current note has played for NOTE_TIME amount of time
  else if(note_timer == NOTE_TIME && tune_elem != 21){
    tune_elem++;
    note_timer = 0;
    tune_timer = 0;
  }

  //if all notes have been played wait 4sec, then go back to beginning of tune[] array
  else if(tune_elem == 21 && tune_timer == 40000){
    tune_elem = 0;
    tune_timer = 0;
    note_timer = 0;
  }

  //kill the current task
  task_self_quit();

  //if the next ask is DEAD, revive it
  if(processList[0].function == 0){
    task_start(REVIVE_SEVSEG_TASK);
  }

  //increment internal timers
  tune_timer++;
  note_timer++;
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
 *    Acknowledgments: LAB 3 starter code
 */
void function_ptr(void* function()){
    function();
}


/**************************************************
 * void task_self_quit()
 *
 *    Arguments - no arguments
 *
 *    Returns - no returns, this is a void function
 *
 *    This function allows a task to terminate itself by manipulating its TCB
 *    A task can call this function to change its own status to DEAD and remove itself
 *    from the tcbList.
 *
 *    Acknowledgments: LAB 3 starter code
 */
void task_self_quit(){
  //set the state of the current task to DEAD
  processList[currentTask].state = DEAD;

  //if the current task is sevseg move the task to deadTaskList
  //and replace all values in processList[0] with NULL
  if (processList[currentTask].pid == 0){
    deadTaskList[0] = *KILL_SEVSEG_TASK;
    memset(&processList[0], 0, sizeof(processList[0]));
    return;
  }

  //if the current task is playSpeajer move the task to deadTaskList
  //and replace all values in processList[1] with NULL
  if (processList[currentTask].pid == 1){
    deadTaskList[1] = *KILL_PLAY_SPEAKER_TASK;
    memset(&processList[1], 0, sizeof(processList[currentTask]));
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
  //change teh DEAD task's state to ready and increment taskRunAmount
  task->state = READY;
  task->taskRunAmount++;

  //if the task is sevseg move the task to processList[0]
  //and replace all values in deadTaskList[0] with NULL
  if (task == REVIVE_SEVSEG_TASK){
    processList[0] = *task;
    memset(REVIVE_SEVSEG_TASK, 0, sizeof(deadTaskList[0]));
    return;
  }

  //if the task is playSpeaker move the task to processList[0]
  //and replace all values in deadTaskList[0] with NULL
  if (task == REVIVE_PLAY_SPEAKER_TASK){
    processList[1] = *task;
    memset(REVIVE_PLAY_SPEAKER_TASK, 0, sizeof(deadTaskList[1]));
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
  //increment to the next task
  currentTask++;

  // resets currentTask to beginning of processList
  if ((currentTask == MAX_PROCESSES)){
    currentTask = 0;
  }

  // otherwise sets task to new task
  // when task is in ready state, execute
  if (processList[currentTask].state == READY) {
    function_ptr(processList[currentTask].function);
  }
}


/**************************************************
 * void sevSegCounter()
 *
 *    Argument - no arguments
 *
 *    Returns - no returns, this is a void function
 *
 *    If there is a note playing then this task will display the
 *    frequency of that note on the sevseg counter. If there is no note
 *    playing then this tasks uses the tune timer to decrement the sevSeg_Counter
 *    every 100ms, and then displays this value on the sevseg display. Once
 *    sevSeg_counter reaches zero, the tune will play again.
 */
void sevSegCounter(){
  //if the tune has NOT completed playing yet
  if (tune_elem != 21){
    sevSeg_Counter = 41;
    sevseg.setNumber(mario[tune_elem], 1); //display the frequency of the current note being played

  //if the tune HAS completed playing
  } else if (tune_elem == 21){
    if (tune_timer == 0){
      sevseg.setNumber(sevSeg_Counter, 1);
    }else if (tune_timer % 1000 == 0) {    //every 100ms decrement sevSeg_Counter and display new value
      sevSeg_Counter--;
      sevseg.setNumber(sevSeg_Counter, 1);
    }
  }

  sevseg.refreshDisplay();

  //kill the current task
  task_self_quit();

  //if the next task is DEAD, revive it
  if(processList[1].function == 0){
    task_start(REVIVE_PLAY_SPEAKER_TASK);
  }

}
