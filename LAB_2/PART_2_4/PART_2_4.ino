/*
* University of Washington
* ECE 474,  7/14/23
* Jason Bentley
*
* Lab 2 - Part 2.4
*
* Acknowledgments: 
* Prof. Makhsous in class example in 7/11/2023, forum.arduino.cc, 
* stackexchange.com, ChatGPT code interpreter plugin
*/

#include <math.h>
#define PIN_OC4A PH3
#define CLOCK_SPEED 16000000

/**************************************************
 * void generate_frequency(int, int)
 *    Argument 1 - the frequency in Hz of the tone you wish to generate. 
 *                 Pass in 0 for silence.
 *    Argument 2 - the length of time in seconds that the tone will play for
 * 
 *    Returns - no returns, this is a void function
 * 
 *    The function takes your desired frequency and the length of time for
 *    which you would like to play the tone. Based on this it then calculates 
 *    what value to set for the OCR4A flag. If 0 is passed for frequency then
 *    a while loop will iterate a counter by the number of seconds passed in
 *    and do nothing for that period of time. Else, another while loop will
 *    change state of the pin every time the timer reaches the OCR4A flag.
 * 
 *    Acknowledgments: Prof. Makhsous in class example in 7/11/2023, ChatGPT code interpreter plugin
*/
void generate_frequency(int frequency_in_Hz, int seconds){
  
  double period_halved = (1.0 / frequency_in_Hz) / 2.0; 
  int flag = round(period_halved / (256.0 / CLOCK_SPEED));
  int time = seconds * 2 * frequency_in_Hz; //time is how long the while loop runs by calculating the number of expected state transistions for a given period of time at a given frequency
  int counter = 0; //counts how many state transistion occur within while loop

  TCCR4B |= (1 << WGM42); //enable CTC mode
  TCCR4B |= (1 << CS42); //set prescaler 256
  TCNT4 = 0; // initialize timer 4 to 0

  if(frequency_in_Hz == 0){

    OCR4A = 62500 * seconds; // 1 / (256/16*10^6)

    while(counter != seconds){
      if (TIFR4 & (1 << OCF4A)){
        TIFR4 |= (1 << OCF4A);
        counter++;
      }
    }

  } else {
    
    OCR4A = flag;

    while(counter < time){
      if (TIFR4 & (1 << OCF4A)){  // if timer interupt register bit 1 is high for timer 4
        TIFR4 |= (1 << OCF4A);    // then toggle that bit to 0
        PORTH ^= (1 << PIN_OC4A); // and set the state of the pin to opposite of what it is now
        counter++;
      }
    }
  } 
}


int main(){
  DDRH |= (1 << PIN_OC4A);

  while(1){
    generate_frequency(400,1);
    generate_frequency(250,1);
    generate_frequency(800,1);
    generate_frequency(0,1);
  }
  return 0;
}

