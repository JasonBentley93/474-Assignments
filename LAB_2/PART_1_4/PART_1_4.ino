/*
*University of Washington
*ECE 474,  7/10/23
*Jason Bentley
*
*Lab 2 - Part 1.4
*
*Acknowledgments: N/A
*/

void setup() {

  DDRL |= (1 << PORTL2); //initialize pin 47
  DDRL |= (1 << PORTL1); //initialize pin 48
  DDRL |= (1 << PORTL0); //initialize pin 49

}

void loop() {
  
  PORTL &= ~(1 << PORTL0); //turn off pin 49
  PORTL |=  (1 << PORTL2); // turn on pin 47
  delay(333);
  PORTL &= ~(1 << PORTL2); // turn off pin 47
  PORTL |=  (1 << PORTL1); //turnb on pin 48
  delay(333);
  PORTL &= ~(1 << PORTL1); //turn off pin 48
  PORTL |=  (1 << PORTL0); //turn on pin 49
  delay(333);
                 
}