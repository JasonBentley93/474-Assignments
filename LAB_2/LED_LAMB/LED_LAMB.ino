/*
* University of Washington
* ECE 474,  7/14/23
* Jason Bentley
*
* Lab 2 - Part 4 - LED Matrix & Little Lamb
*
* Acknowledgments: Ishaan Bhimani, ChatGPT code interpreter plug in
*/

#define NOTE_C 261
#define NOTE_D 294
#define NOTE_E 329
#define NOTE_F 349
#define NOTE_G 392
#define NOTE_A 440
#define NOTE_B 493
#define NOTE_C 523
#define NOTE_R 5

#define OP_DECODEMODE  8
#define OP_SCANLIMIT   10
#define OP_SHUTDOWN    11
#define OP_DISPLAYTEST 14
#define OP_INTENSITY   10

#define PIN_OC4A PH3


int melody[] = { NOTE_E, NOTE_R, NOTE_D, NOTE_R, NOTE_C, NOTE_R, NOTE_D, NOTE_R, NOTE_E, 
NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_D, NOTE_R,NOTE_D, NOTE_R,NOTE_E, 
NOTE_R,NOTE_G,NOTE_R,NOTE_G, NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_C, NOTE_R,NOTE_D, 
NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_D, 
NOTE_R,NOTE_E, NOTE_R,NOTE_D, NOTE_R,NOTE_C, NOTE_R,NOTE_C };

volatile int uS_counter  = 0;
volatile int ms_counter  = 0;

volatile bool tone_flag = true;
volatile int tone_counter  = 0;
volatile int tone_state    = 0;

int x_axis        = 520;
int y_axis        = 520;

// a global array to store the columns of the LED Matrix
byte column[] = {B10000000, B01000000, B00100000, B00010000, B00001000, B00000100, B00000010, B00000001};

//Transfers 1 SPI command to LED Matrix for given row
//Input: row - row in LED matrix
//       data - bit representation of LEDs in a given row; 1 indicates ON, 0 indicates OFF
void spiTransfer(volatile byte row, volatile byte data);

// change these pins as necessary
int DIN = 12;
int CS =  11;
int CLK = 10;

byte spidata[2]; //spi shift register uses 16 bits, 8 for ctrl and 8 for data

void setup(){

  //must do this setup
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);

  DDRH |= (1 << PIN_OC4A);  //initialize pin 6
  
  digitalWrite(CS, HIGH);
  spiTransfer(OP_DISPLAYTEST,0);
  spiTransfer(OP_SCANLIMIT,7);
  spiTransfer(OP_DECODEMODE,0);
  spiTransfer(OP_SHUTDOWN,1);

  //clear the matrix
  for (int i = 0; i < 8; i++)
    spiTransfer(i,B00000000);

}

void loop(){

    // uS counter increments every 100uS
    uS_counter++;
    
    // delay function is 100 uS, therefore 100uS * 10 = 1ms
    if(uS_counter ==10){
        ms_counter++;
        uS_counter = 0;
    }

    generate_tone();

  // read the joystick ADC and update the LED matrix every 50 ms
  if (ms_counter == 50){
    
    // clear the LED matrix
    for (int i = 0; i < 8; i++)
        spiTransfer(i,B00000000);
    

    /*********
    * X AXIS *
    **********/
    int x_axis = analogRead(A0);
  
    //map 0-1023 to 0-8. 8 is set as max because the joystick looses sensitivity towards
    //the edges of its travel distances in X and Y directions
    x_axis = map(x_axis, 0, 1023, 0, 8); 
  
    //Increase the sensitivity of the edges of the joysticks travel
    if (x_axis == 8)
        x_axis = 7;


    /*********
    * Y AXIS *
    **********/
    int y_axis = analogRead(A1);

    //map 0-1023 to 0-8. 8 is set as max because the joystick looses sensitivity towards
    //the edges of its travel distances in X and Y directions
    y_axis = map(y_axis, 0, 1023, 0, 8);

    //Increase the sensitivity of the edges of the joysticks travel
    if (y_axis == 8)
        y_axis = 7;
  
    spiTransfer(y_axis, column[x_axis]);
    ms_counter = 0;
  }
  delayMicroseconds(100); //sync delay function acts as the global clock
}


void spiTransfer(volatile byte opcode, volatile byte data){
  int offset = 0; //only 1 device
  int maxbytes = 2; //16 bits per SPI command
  
  for(int i = 0; i < maxbytes; i++) { //zero out spi data
    spidata[i] = (byte)0;
  }
  //load in spi data
  spidata[offset+1] = opcode+1;
  spidata[offset] = data;
  digitalWrite(CS, LOW); //
  for(int i=maxbytes;i>0;i--)
    shiftOut(DIN,CLK,MSBFIRST,spidata[i-1]); //shift out 1 byte of data starting with leftmost bit
  digitalWrite(CS,HIGH);
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