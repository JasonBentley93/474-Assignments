/*
* University of Washington
* ECE 474,  7/14/23
* Jason Bentley
*
* Lab 2 - Part 4 - LED Matrix
*
* Acknowledgments: Ishaan Bhimani wrote the majoirty of this code
*/

#define OP_DECODEMODE  8
#define OP_SCANLIMIT   10
#define OP_SHUTDOWN    11
#define OP_DISPLAYTEST 14
#define OP_INTENSITY   10

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

  /*
  * X AXIS
  */
  int x_axis = analogRead(A0);
  
  //map 0-1023 to 0-8. 8 is set as max because the joystick looses sensitivity towards
  //the edges of its travel distances in X and Y directions
  x_axis = map(x_axis, 0, 1023, 0, 8); 
  
  //Increase the sensitivity of the edges of the joysticks travel
  if (x_axis == 8)
    x_axis = 7;
  

  /*
  * Y AXIS
  */
  int y_axis = analogRead(A1);

  //map 0-1023 to 0-8. 8 is set as max because the joystick looses sensitivity towards
  //the edges of its travel distances in X and Y directions
  y_axis = map(y_axis, 0, 1023, 0, 8);

  //Increase the sensitivity of the edges of the joysticks travel
  if (y_axis == 8)
    y_axis = 7;
  
  spiTransfer(y_axis, column[x_axis]);
  delay(50);
  spiTransfer(y_axis,B00000000); //reset the LED in the matrix
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
