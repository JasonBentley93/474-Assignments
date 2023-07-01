/* University of Washington
ECE 474,  6/27/23
*
Jason Bentley


Lab 1 - Part 5.1


Acknowledgments:	Blink.ino from example sketches & "Blink Without Delay" from Arduino community forums.
*/

int state_GPIO10 = HIGH;  //Initialize pin 10 (external LED) state to HIGH
int state_GPIO13 = LOW;   //Initialize pin 13 (onboard LED) state to LOW
int state_GPIO2  = LOW;   //Initialize pin 2 (speaker) state to LOW
int counter      = 0;     //counter will count the number of times pin 2 goes HIGH

unsigned long prevTimeLED     = 0; //initialize an empty variable for keeping track of LED timing
unsigned long prevTimeSpeaker = 0; //initialize an empty variable for keeping track of Speaker timing

const long intervalLED     = 200; // set LED interval to 200ms
const long intervalSpeaker = 2;   // set Speaker interval to 2ms

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin GPIO 10, GPIO 2, and GPIO13 as an output.
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  
  unsigned long currentTime = millis();  //Get current time in milliseconds
  
  // This if statement compares a timer to a constant in order to find how long the speaker has 
  // been in any particular state. If a certain threshold (set by intervalSpeaker) is met then this 
  // if statement will change the state of the speaker.
  if ((currentTime - prevTimeSpeaker >= intervalSpeaker) & (counter < 2000)) // If the difference between the current time and the previous time is >= to 2ms and the counter is less than 4 seconds
  {
    prevTimeSpeaker = currentTime; // set the prevTimeSpeaker variable to whatever the current time is
    counter += 1; //increment the counter by 1
    
    // This if/else statement checks the current state of the speaker and then sets it to the opposite state creating a "click"
    if (state_GPIO2 == LOW)
    {
      state_GPIO2 = HIGH;
    } else {
      state_GPIO2 = LOW;
    }
    digitalWrite(2, state_GPIO2);
  }
  
  // This if statement compares a timer to a constant in order to find how long the LED has 
  // been in any particular state. If a certain threshold (set by intervalLED) is met then this 
  // if statement will change the state of the LEDs.
  if (currentTime - prevTimeLED >= intervalLED) //If the difference between the current time and the previous time is >= to 200ms
  {
    prevTimeLED = currentTime; // set the prevTimeLED variable to whatever the current time is
    
    // This if/else statement checks the current state of the LEDs and then sets it to the opposite state.
    if (state_GPIO13 == LOW)
    {
      state_GPIO13 = HIGH;
      state_GPIO10 = LOW;
    } else {
      state_GPIO13 = LOW;
      state_GPIO10 = HIGH;
    }
    digitalWrite(10, state_GPIO10);
    digitalWrite(13, state_GPIO13);
  }
}
