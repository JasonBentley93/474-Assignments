/*
* University of Washington
* ECE 474,  7/14/23
* Jason Bentley
*
* Lab 2 - Part 3.3
*
* Acknowledgments: 
* Prof. Makhsous in class example in 7/11/2023, forum.arduino.cc, 
* stackexchange.com, ChatGPT code interpreter plugin
*/

#include <math.h>
#define NOTE_C 261
#define NOTE_D 294
#define NOTE_E 329
#define NOTE_F 349
#define NOTE_G 392
#define NOTE_A 440
#define NOTE_B 493
#define NOTE_C 523
#define NOTE_R 5


int melody[] = { NOTE_E, NOTE_R, NOTE_D, NOTE_R, NOTE_C, NOTE_R, NOTE_D, NOTE_R, NOTE_E, 
NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_D, NOTE_R,NOTE_D, NOTE_R,NOTE_E, 
NOTE_R,NOTE_G,NOTE_R,NOTE_G, NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_C, NOTE_R,NOTE_D, 
NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_D, 
NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_C, NOTE_R,NOTE_C };

// uncomment to hold each note longer 
// int melody[] = { NOTE_E, NOTE_E, NOTE_R, NOTE_D, NOTE_D, NOTE_R, NOTE_C, NOTE_C, NOTE_R, NOTE_D, NOTE_D, NOTE_R, NOTE_E, NOTE_E, 
// NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_D, NOTE_D, NOTE_R,NOTE_D, NOTE_D, NOTE_R,NOTE_D, NOTE_D, NOTE_R,NOTE_E, NOTE_E, 
// NOTE_R,NOTE_G, NOTE_G,NOTE_R,NOTE_G, NOTE_G, NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_D, NOTE_D, NOTE_R,NOTE_C, NOTE_C, NOTE_R,NOTE_D, NOTE_D, 
// NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_D, NOTE_D, NOTE_R,NOTE_D, NOTE_D, 
// NOTE_R,NOTE_E, NOTE_E, NOTE_R,NOTE_D, NOTE_D, NOTE_R,NOTE_C, NOTE_C, NOTE_R,NOTE_C, NOTE_C };

#define PIN_OC4A PH3
#define LED_PIN47 PL2
#define LED_PIN48 PL1
#define LED_PIN49 PL0

volatile int LED_state   = 0;

volatile int tone_counter  = 0;
volatile int tone_state    = 0;

volatile int uS_counter  = 0;
volatile int ms_counter  = 0;
volatile int sec_counter = 0;

volatile bool LED_flag  = false;
volatile bool tone_flag = false;


/**************************************************
 * void generate_pattern()
 *    Argument 1 - No arguments
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function watches the sec_counter global variable and sets flags
 *    and controls the orders of operations for blinking the LEDS
 *    and controlling the tones played by the speaker.
 * 
 *    Acknowledgments: N/A
*/ 
void generate_pattern(){

    if (sec_counter < 2){
        LED_flag = true;
    } else if (sec_counter >= 2 && sec_counter <= 6){
        LED_flag  = false;
        tone_flag = true;
    } else if (sec_counter > 6 && sec_counter < 16){
        LED_flag = true;
    } else if (sec_counter >= 16){
        LED_flag = false;
        tone_flag = false;
    }

    generate_LED();
    generate_tone();

}


/**************************************************
 * void generate_LED()
 *    Argument 1 - No arguments
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function watches the ms_counter global variable and the LED_flag
 *    in order to control the sequence of the LEDs. This task is hard coded for
 *    a 3Hz blinking sequence.
 * 
 *    Acknowledgments: Prof. Makhsous in class example in 7/11/2023
*/
void generate_LED(){
    if(LED_flag){
        if (ms_counter == 333){
            PORTL &= ~(1 << LED_PIN49); //turn off pin 49
            PORTL |=  (1 << LED_PIN47); // turn on pin 47
            LED_state = 1;
        } else if (ms_counter == 666){
            PORTL &= ~(1 << LED_PIN47); // turn off pin 47
            PORTL |=  (1 << LED_PIN48); //turn on pin 48
            LED_state = 2;
        } else if (ms_counter == 999){
            PORTL &= ~(1 << LED_PIN48); //turn off pin 48
            PORTL |=  (1 << LED_PIN49); //turn on pin 49
            LED_state = 0;
        }
    }

    if(!LED_flag){
        PORTL &= ~(1 << LED_PIN49);
        PORTL &= ~(1 << LED_PIN48);
        PORTL &= ~(1 << LED_PIN47);
        LED_state = 0;
    }
}


/**************************************************
 * void generate_tone()
 *    Argument 1 - No arguments
 * 
 *    Returns - no returns, this is a void function
 * 
 *    This function watches the tone_flag global variable and controls the
 *    sequence of speaker tones. The tones are hard coded in a global
 *    array to play Marry Had a Little Lamb. Additionally, tones are hard 
 *    coded to play for 1/5 second each.
 * 
 *    Acknowledgments: ChatGPT code interpreter plug-in
*/
void generate_tone(){
    static int time;
    static int half_period;
    static int melody_length = sizeof(melody) / sizeof(melody[0]);

    if(tone_flag){
        time++;

        if (tone_state < melody_length) // Check to see position in melody array
            half_period = round((5000 / melody[tone_state])); //calculate half period of tone
        else
            half_period = 5000; 
       
        // Toggle the state of the OC4A pin on each half period
        if (time == half_period){
            PORTH ^= (1 << PIN_OC4A);
            tone_counter++;
            time = 0;
        }

        // Check to see if we have reached the end of the melody array. And 
        // calcualte how long the tone has been playing and change the tone state
        // when after the tone has been playing for set period of time
        if (tone_counter == melody[tone_state]/5 && tone_state < melody_length){
            tone_counter = 0;
            tone_state++;
        } else if (tone_state == melody_length){
            tone_counter = 0;
            tone_state = 0;
        }   
    }

    // Reset tone_state and tone_counter to zero so that 
    // tones restart at beginning of sequence.
    if(!tone_flag){
        tone_state = 0;
        tone_counter = 0;
    }
}


void setup(){
  DDRL |= (1 << LED_PIN47); //initialize pin 47
  DDRL |= (1 << LED_PIN48); //initialize pin 48
  DDRL |= (1 << LED_PIN49); //initialize pin 49
  DDRH |= (1 << PIN_OC4A);  //initialize pin 6
}


void loop(){
    
    //uS counter increments every 100uS
    uS_counter++;
    
    //delay function is 100 uS, therefore 100uS * 10 = 1ms
    if(uS_counter ==10){
        ms_counter++;
        uS_counter = 0;
    }
    
    if(ms_counter == 1000){
        sec_counter++;
        ms_counter = 0;
    }

    if (sec_counter == 17){
        sec_counter = 0;
    }

    generate_pattern();

    delayMicroseconds(100); //sync delay function acts as the global clock

}