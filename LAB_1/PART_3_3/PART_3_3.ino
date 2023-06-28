/* University of Washington
ECE 474,  6/27/23
*
Jason Bentley


Lab 1 - Part 3.3


Acknowledgments:	Blink.ino from example sketches
*/

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin GPIO 10 as an output.
  pinMode(10, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(10, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);               // wait for a 200ms
  digitalWrite(10, LOW);    // turn the LED off by making the voltage LOW
  delay(200);               // wait for a 200ms
}
