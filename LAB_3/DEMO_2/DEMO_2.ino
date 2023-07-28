/*
* University of Washington
* ECE 474,  7/18/23
* Jason Bentley
*
* Lab 3 - DEMO 2
*
* Acknowledgments: LAB 3 starter code from Sep Makhsous
*/

// Define necessary variables and constants
#define LED_PIN  7
#define TONE_PIN 10

//define max size of task array
#define MAX_SIZE 10

//define the possible states a task can have
#define READY    0
#define RUNNING  1
#define SLEEPING 2
#define PENDING  4
#define DONE     5

//define the notes for the tune
#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

//100ms -> based on a 100us delay, the length of time each note should play
#define NOTE_TIME  1000 //100ms

int LED_timer = 0; //counter to store length of time which LED has been in a particular state
int LED_state = 0; //counter to store the current state of the LED

long tune_timer   = 0; //counter to store length of time which speaker has been in a particular state
int tune_elem     = 0; //counter to store the the position of the current note in the mario[] array
int tune_state    = 0; //counter to store the current state of the speaker
int note_timer    = 0; //counter to store the length of time which a note has been playing

int timerCounter   = 0; //a global counter acting as a timer
volatile int sFlag = 0; //a flag rasied on interupt
long x = 0;             //a dummy variable for schedule_sync() to do nothing wile sleeping

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};

//initialize a pointer to the array holding the tasks
void (*taskScheduler[MAX_SIZE]) = {0};

int taskSleep[MAX_SIZE]; //Initialize an array for storing sleeping tasks
int taskState[MAX_SIZE]; //Initialize an array for storing non-sleeping tasks
int currentTask = 0;     //an int to hold the position of the current running task in the array



void setup() {
  // Set up pins as outputs for the speaker and LED
  pinMode(LED_PIN,  OUTPUT);
  pinMode(TONE_PIN,  OUTPUT);

  //populate task array with tasks
  taskScheduler[0] = flashLED;
  taskScheduler[1] = playSpeaker;
  taskScheduler[2] = schedule_sync;
  taskScheduler[3] = NULL;
  
  //Set up timer and its configurations for ISR (CTC mode, 10KHz, prescaler = 64)
  TCCR4A = 0;
  TCCR4B = (1 << WGM42) | (1 << CS41) | (1 << CS40);
  OCR4A = 25; //100uS interupt, 10Khz
  TIMSK4 |= (1 << OCIE4A);
  sei();
}


void loop() {
  // Enter the loop when a task is present in the taskScheduler
  while (taskScheduler[currentTask] != NULL) {
    
    // Run the task if it is not in the SLEEPING state
    if (taskState[currentTask] != SLEEPING){
        function_ptr(taskScheduler[currentTask]);
        
        // Change the task state to RUNNING
        taskState[currentTask] = RUNNING;
    }
    
    //increment to next task and timerCounter "clock"
    currentTask++;
    timerCounter++;
  }

  //go back to beginning of task array and increment task-specific timers
  currentTask = 0;
  tune_timer++;
  note_timer++;
  LED_timer ++;
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
void flashLED(void *p){
    if(LED_state == LOW && LED_timer == 7500 || LED_timer == 0){
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
void playSpeaker(void *p) {
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
  if(tune_elem == 21 && tune_timer == 40000){
    tune_elem = 0;
    tune_timer = 0;
    note_timer = 0;
    return;
  }

}


/**************************************************
 * void sleep_474(int)
 * 
 *    Argument - The sleep time in milliseconds
 * 
 *    Returns - no returns, this is a void function
 * 
 *    Function that controls the sleep time of a task
 * 
 *    Acknowledgments: LAB 3 starter code
 */
void sleep_474(int t) {
  // Set the sleep time of the current task
  taskSleep[currentTask] = t;
  
  // Change the task state to SLEEPING
  taskState[currentTask] = SLEEPING;
}


/**************************************************
 * void schedule_sync()
 * 
 *    Argument - no arguments
 * 
 *    Returns - no returns, this is a void function
 * 
 *    Function that synchronizes the schedule and tasks while in the PENDING state
 * 
 *    Acknowledgments: LAB 3 start code
 */
void schedule_sync(void *p) {
  // Enter an infinite loop when sFlag is PENDING
  while(sFlag == PENDING){
    x += 1;
  }
  // Update the sleep time of the task
  sleep_474(timerCounter);
  taskSleep[currentTask] -= 2;
  // Wake up any sleeping task if the sleep time is negative
  if (taskSleep[currentTask] < 0){
    taskState[currentTask] = READY;
  }
  // Reset the sFlag to PENDING
  sFlag = PENDING;
}


/**
 * Interrupt Service Routine for TIMER0_COMPA_vect
 * Sets the flag to DONE
 */
ISR(TIMER4_COMPA_vect) {
  sFlag = DONE;
  TIFR4 |= (1 << OCF4A);
}