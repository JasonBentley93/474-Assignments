/* University of Washington
ECE 474,  6/27/23
*
Jason Bentley


Lab 1 - Part 4.3


Acknowledgments:	Blink.ino from example sketches
*/

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin GPIO 10, GPIO13, and GPIO 2 as an output.
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(10, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(13, LOW);    // turn the LED on (LOW is the voltage level)
  digitalWrite(2, LOW);     // speaker click
  delay(200);               // wait for a 200ms
  digitalWrite(10, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(2, HIGH);    // speaker click
  delay(200);               // wait for a 200ms
}
