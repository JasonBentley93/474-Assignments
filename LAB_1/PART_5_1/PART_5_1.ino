/* University of Washington
ECE 474,  6/27/23
*
Jason Bentley


Lab 1 - Part 5.1


Acknowledgments:	Blink.ino from example sketches & "Blink Without Delay" from Arduino community forums.
*/

int state_GPIO10 = HIGH;
int state_GPIO13 = LOW;
int state_GPIO2  = LOW;
int counter      = 0;    //counter will count how long the speaker has been playing

unsigned long prevTimeLED     = 0;
unsigned long prevTimeSpeaker = 0;

const long intervalLED     = 200; // set interval to 200ms
const long intervalSpeaker = 2;   // set interval to 2ms

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin GPIO 10, GPIO 2, and GPIO13 as an output.
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  unsigned long currentTime = millis();  //Get current time
  if ((currentTime - prevTimeSpeaker >= intervalSpeaker) & (counter < 2000)) //If the difference between the current time and the previous time is >= to 2ms and counter is less than 4 seconds
  {
    prevTimeSpeaker = currentTime;
    counter += 1;
    if (state_GPIO2 == LOW)
    {
      state_GPIO2 = HIGH;
    } else {
      state_GPIO2 = LOW;
    }
    digitalWrite(2, state_GPIO2);
  }

  if (currentTime - prevTimeLED >= intervalLED) //If the difference between the current time and the previous time is >= to 200ms
  {
    prevTimeLED = currentTime;
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
