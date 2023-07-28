/*
* University of Washington
* ECE 474,  7/18/23
* Jason Bentley
*
* Lab 3 - DEMO 3
*
* Acknowledgments: LAB 3 starter code from Sep Makhsous
*/

// Define necessary variables and constants
#define LED_PIN  7
#define TONE_PIN 10

//define max size of task struct array
#define MAX_PROCESSES 2

//define the possible states a task can have
#define READY 1
#define DEAD  0

//define the notes for the tune
#define NOTE_E 659
#define NOTE_C 523
#define NOTE_G 784
#define NOTE_g 392
#define NOTE_R 0

//define specefic elements in the deadTaskList that will hold respective tasks
#define REVIVE_FLASH_LED_TASK    &deadTaskList[0]
#define REVIVE_PLAY_SPEAKER_TASK &deadTaskList[1]

//define specefic elements in the processList that will hold respective tasks
#define KILL_FLASH_LED_TASK    &processList[0]
#define KILL_PLAY_SPEAKER_TASK &processList[1]

//100ms -> based on a 100us delay, the length of time each note should play
#define NOTE_TIME   1000 //100ms


int LED_timer = 0; //counter to store length of time which LED has been in a particular state
int LED_state = 0; //counter to store the current state of the LED

long tune_timer   = 0; //counter to store length of time which speaker has been in a particular state
int tune_elem     = 0; //counter to store the the position of the current note in the mario[] array
int tune_state    = 0; //counter to store the current state of the speaker
int note_timer    = 0; //counter to store the length of time which a note has been playing

int sFlag       = 1; //a flag rasied on interupt
int currentTask = 0; //an int to hold the position of the current running task in the array

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


void setup() {
  // Set up pins as outputs for the speaker and LED
  pinMode(LED_PIN,  OUTPUT);
  pinMode(TONE_PIN, OUTPUT);

  // Initialize characteristics of tasks in the processList array
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

//Set up timer and its configurations for ISR (CTC mode, 10KHz, prescaler = 64)
  TCCR4A  = 0;
  TCCR4B  = (1 << WGM42) | (1 << CS41) | (1 << CS40);
  OCR4A   = 25; //100uS interupt, 10Khz
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

  if(tune_timer == period_half && tune_elem != 21){
    //if the speaker is off
    if(tune_state == LOW){
      digitalWrite(TONE_PIN, HIGH);
      tune_timer = 0;
      tune_state = HIGH;

    //if the speakers it on
    }else if(tune_state == HIGH){
      digitalWrite(TONE_PIN, LOW);
      tune_timer = 0;
      tune_state = LOW;
    }
  }

  //increment to next note if current note has played for NOTE_TIME amount of time
  if(note_timer == NOTE_TIME && tune_elem != 21){
    tune_elem++;
    note_timer = 0;
    tune_timer = 0;

  //if all notes have been played wait 4sec, then go back to beginning of tune[] array
  }else if(tune_elem == 21 && tune_timer == 40000){
    tune_elem = 0;
    tune_timer = 0;
    note_timer = 0;
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

  //if the current task is flashLED move teh task to deadTaskList
  //and replace all values in processList[0] with NULL
  if (processList[currentTask].pid == 0){
    deadTaskList[0] = *KILL_FLASH_LED_TASK;
     memset(&processList[currentTask], 0, sizeof(processList[currentTask]));
    return;
  }

  //if the current task is playSpeajer move the task to deadTaskList
  //and replace all values in processList[1] with NULL
  if (processList[currentTask].pid == 1){
    deadTaskList[1] = *KILL_PLAY_SPEAKER_TASK;
     memset(&processList[currentTask], 0, sizeof(processList[currentTask]));
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
  //change teh DEAD task's state to ready and increment raskRunAmount
  task->state = READY;
  task->taskRunAmount++;

  //if the task is flashLED move the task to processList[0]
  //and replace all values in deadTaskList[0] with NULL
  if (task == REVIVE_FLASH_LED_TASK){
    processList[0] = *task;
    memset(REVIVE_FLASH_LED_TASK, 0, sizeof(deadTaskList[0]));
    return;
  }

  //if thetask is flashLED move the task to processList[0]
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
  // resets currentTask to beginning of processList
  if ((currentTask == MAX_PROCESSES)){
    currentTask = 0;
  }

  // when task is in ready state, execute
  if (processList[currentTask].state != 0) {
    function_ptr(processList[currentTask].function);
  }

  // increment currentTask to traverse processList
  currentTask++;

  //on interupt increment task-internal counters
  if (sFlag == 1){
    LED_timer++;
    tune_timer++;
    note_timer++;
    sFlag = 0;
  }
}

/**
 * Interrupt Service Routine for TIMER0_COMPA_vect
 * Sets the flag to DONE
 */
ISR(TIMER4_COMPA_vect) {
  sFlag = 1;
  TIFR4 |= (1 << OCF4A);
}
