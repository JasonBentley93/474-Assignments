/*
* University of Washington
* ECE 474,  7/18/23
* Jason Bentley
*
* Lab 3 - DEMO 1
*
* Acknowledgments: LAB 3 starter code from Sep Makhsous
*/

#define LED_PIN  7
#define TONE_PIN 10

//define the notes for the tune
#define NOTE_E 659 
#define NOTE_C 523 
#define NOTE_G 784 
#define NOTE_g 392 
#define NOTE_R 0  

//100ms -> based on a 100us delay, the length of time each note should play
#define NOTE_TIME  1000

int LED_timer = 0; //counter to store length of time which LED has been in a particular state
int LED_state = 0; //counter to store the current state of the LED

long tune_timer   = 0; //counter to store length of time which speaker has been in a particular state
int tune_elem     = 0; //counter to store the the position of the current note in the mario[] array
int tune_state    = 0; //counter to store the current state of the speaker
int note_timer    = 0; //counter to store the length of time which a note has been playing

int mario[] = {NOTE_E, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_E, NOTE_R, NOTE_R, NOTE_C, NOTE_R,
              NOTE_E, NOTE_R, NOTE_R, NOTE_G, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_R, NOTE_g, 
              NOTE_R};

             

void setup(){
    pinMode(LED_PIN,  OUTPUT);
    pinMode(TONE_PIN,  OUTPUT);
}


void loop() {
  while (1) {
    //flash the external LED
    flashLED();
    
    //play the mario tuner
    playSpeaker(mario);
    
    //increment all timers
    tune_timer++;
    note_timer++;
    LED_timer ++;
    
    //delay
    delayMicroseconds(100);
    }
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
 * void playSpeaker(int[])
 * 
 *    Argument - the int array holding the notes of a tune in the
 *               order which they will be played.
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function calculates the half-period of a given frequency 
 *    in the passed int array. It then itterates through this array
 *    and plays each note for a NOTE_TIME amount of time. After all
 *    of the notes are played there is a hardcoded 4sec pause.
 * 
 *    Acknowledgments: N/A
 */
void playSpeaker(int tune[]) { 
    //calculate the half period of a frequency
    int period_half = 5000/(tune[tune_elem] + 1);

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


