/*
* University of Washington
* ECE 474,  7/14/23
* Jason Bentley
*
* Lab 2 - Part 3.2
*
* Acknowledgments: 
* Prof. Makhsous in class example in 7/11/2023, forum.arduino.cc, 
* stackexchange.com, ChatGPT code interpreter plugin
*/

#include <math.h>

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
    } else if (sec_counter >= 2 && sec_counter < 6){
        LED_flag  = false;
        tone_flag = true;
    } else if (sec_counter >= 6 && sec_counter < 16){
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

    // Turn off LEDs 
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
 *    sequence of speaker tones. The tones are hard coded to be 400Hz,
 *    250Hz, 800Hz and 0Hz. Additionally, tones are hard coded to play
 *    for one second each.
 * 
 *    Acknowledgments: N/A
*/
void generate_tone(){
    static int time;
    static int half_period;

    int frequency[] = {400, 250, 800, 0};
    
    if(tone_flag){
        time++;
        
        // Depending on tone_state value, calculate the 1/2 * period of the signal
        if (tone_state == 0)
            half_period = round((5000/ frequency[0]));
        else if (tone_state == 1)
            half_period = round((5000 / frequency[1]));
        else if (tone_state == 2)
            half_period = round((5000 / frequency[2]));
        else if (tone_state == 3){
            half_period = 5000;
        }

        // Toggle the state of the OC4A pin on each half period
        if (time == half_period){
            PORTH ^= (1 << PIN_OC4A);
            tone_counter++;
            time = 0;
        }

        // Calcualte how long the tone has been playing and change the tone state
        // when after the tone has been playing for set period of time
        if (tone_counter == 2 * frequency[0] && tone_state == 0){
            tone_counter = 0;
            tone_state = 1;
        } else if (tone_counter == 2 * frequency[1] && tone_state == 1){
            tone_counter = 0;
            tone_state = 2;
        } else if (tone_counter == 2 * frequency[2] && tone_state == 2){
            tone_counter = 0;
            tone_state = 3;
        } else if (tone_counter == 2 && tone_state == 3){
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
    if(uS_counter == 10){
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