/**
 * @file Lab3_SRRI.ino
 * @brief ECE Lab 3 Assignment
 * @detail University of Washington ECE 474
 */

// Define necessary variables and constants
#define LED_PIN  7
#define TONE_PIN 10

#define MAX_SIZE 10

#define READY    0
#define RUNNING  1
#define SLEEPING 2
#define PENDING  4
#define DONE     5

#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

#define NOTE_TIME  1000 //100ms

int LED_timer = 0;
int LED_state = 0;

long tune_timer   = 0;
int tune_elem     = 0;
int tune_state    = 0;
int note_timer   = 0;

int timerCounter   = 0;
volatile int sFlag = 0;
long x = 0;

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};

void (*taskScheduler[MAX_SIZE]) = {0};

int taskSleep[MAX_SIZE];
int taskState[MAX_SIZE];
int currentTask = 0;


/**
 * Set up pins and timers for speaker and LED
 * Initialize the task structure for the ISR
 */
void setup() {
  // Set up pins as outputs for the speaker and LED
  pinMode(LED_PIN,  OUTPUT);
  pinMode(TONE_PIN,  OUTPUT);

  taskScheduler[0] = flashLED;
  taskScheduler[1] = playSpeaker;
  taskScheduler[2] = schedule_sync;
  taskScheduler[3] = NULL;
  
  // Set up timers and their configurations for speaker

  TCCR4A = 0;
  TCCR4B = (1 << WGM42) | (1 << CS41) | (1 << CS40);
  OCR4A = 25;

  TIMSK4 |= (1 << OCIE4A);

  sei();

}


/**
 * Loop function that controls the tasks running based on their states
 */
void loop() {
  // Enter the loop when a task is present in the taskScheduler
  while (taskScheduler[currentTask] != NULL) {
    // Run the task if it is not in the SLEEPING state
    if (taskState[currentTask] != SLEEPING){
        function_ptr(taskScheduler[currentTask]);
        // Change the task state to RUNNING
        taskState[currentTask] = RUNNING;
    }
    currentTask++;
    timerCounter++;
  }
  currentTask = 0;
  tune_timer++;
  note_timer++;
  LED_timer ++;
}


/**
 * Function for flashing the external LED
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


void function_ptr(void* function()){
    function();
}


/**
 * Function for playing the theme song on the speaker
 */
void playSpeaker(void *p) { 
    int period_half = 5000/(mario[tune_elem] + 1);

    if(tune_state == LOW && tune_timer == period_half && tune_elem != 21){
    
        digitalWrite(TONE_PIN, HIGH);
        tune_timer = 0;
        tune_state = HIGH;
        return;
    }

    if(tune_state == HIGH && tune_timer == period_half && tune_elem != 21){
        digitalWrite(TONE_PIN, LOW);
        tune_timer = 0;
        tune_state = LOW;
        return;
    }

    if(note_timer == NOTE_TIME && tune_elem != 21){
        // Serial.println("entered function");
        tune_elem++;
        note_timer = 0;
        tune_timer = 0;
        return;
    }

    if(tune_elem == 21 && tune_timer == 40000){
        tune_elem = 0;
        tune_timer = 0;
        note_timer = 0;
        return;
    }

}


/**
 * Function that controls the sleep time of a task
 * @param t The sleep time in milliseconds
 */
void sleep_474(int t) {
  // Set the sleep time of the current task
  taskSleep[currentTask] = t;
  // Change the task state to SLEEPING
  taskState[currentTask] = SLEEPING;
}


/**
 * Function that synchronizes the schedule and tasks while in the PENDING state
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