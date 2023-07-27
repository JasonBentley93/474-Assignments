
/**
 * @file Lab3_RR.ino
 * @brief ECE Lab 3 Assignment
 * @detail University of Washington ECE 474
 */


// Define necessary variables and constants

#define LED_PIN  7
#define TONE_PIN 10

#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

#define NOTE_TIME  1000 //100ms

int LED_timer = 0;
int LED_state = 0;

volatile long tune_timer   = 0;
volatile int tune_elem     = 0;
volatile int tune_state    = 0;
volatile int note_timer    = 0;

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};

             

/**
 * Set up pins and timers for speaker and LED
 */
void setup(){
    pinMode(LED_PIN,  OUTPUT);
    pinMode(TONE_PIN,  OUTPUT);
}

/**
 * Loop function that controls the Round-Robin schedule
 */
void loop() {
  while (1) {
    // Call the flashExternalLED function
    flashLED();

    // Call the playSpeaker function
    playSpeaker(mario);
    tune_timer++;
    note_timer++;
    LED_timer ++;

    // Delay for a short period of time
    delayMicroseconds(82);
    }
}

/**
 * Function for flashing the external LED
 */
void flashLED(){
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

/** 
 * Function for playing the theme song on the speaker 
 */ 
void playSpeaker(int tune[]) { 
    int period_half = 5000/(tune[tune_elem] + 1);

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


