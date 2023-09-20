/**********************************************************************/
/*This code is not clean and OFFCOURSE will be possible do it better! */ 
/*this is only a group of functions to be used as independent code,   */ 
/*and by this way will be possible to any person can do the changes   */ 
/*of code and see what happen.                                        */
/*The code don't depend of any external library or extenal functions  */
/*complicated.                                                        */
/*I'm let the values to sent as binnary, this allow swap bit by bit   */
/* to is possible test segment by segment without convert to HEX      */
/**********************************************************************/
/* This code show how is possible work with the driver LC75824 Sony */
/* This IC same times is marked as only the "366-34" from Sony */
/* Also pinout is equivalent to the PT6524, check the datasheet */
/*Begining of Auto generated code by Atmel studio */
#define VFD_in 8  // This is the pin number 8 on Arduino UNO  // You can use a resistor of 1Kohms to protect this line!
#define VFD_clk 9 // This is the pin number 9 on Arduino UNO  // You can use a resistor of 1Kohms to protect this line!
#define VFD_ce 10 // This is the pin number 10 on Arduino UNO // You can use a resistor of 1Kohms to protect this line!

unsigned int shiftBit=0;
unsigned int nBitOnBlock=0; // Used to count number of bits and split to 8 bits... (number of byte)
unsigned int nByteOnBlock=0; 
unsigned int sequencyByte=0x00;
byte Aa,Ab,Ac,Ad,Ae,Af,Ag,Ah,Ai,Aj;
byte blockBit =0x00;
unsigned char nGrid = 0x00; 
unsigned int twoBytes = 0x0000; //The arduino variable INT have a size of 2 Bytes!!!
unsigned char gridMore2bits =0x00;

//constants won't change. They're used here to set pin numbers:
const int ledPin = 11;    // the number of the LED pin

#define BUTTON_PIN 2 //Att check wich pins, of your platform, accept interrupts!!! Uno is 2 & 3, also confirm voltage of your platform, DUE is 3,3V!!!

volatile byte buttonReleased = false;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

bool forward = false;
bool backward = false;
bool isRequest = true;
bool allOn=false;
bool cycle=false;
/*
//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.
/*************************************************/
void setup() {
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_ce, OUTPUT);
//initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
//initialize the pushbutton pin as an input:
pinMode(BUTTON_PIN, INPUT);
//Next line is the attach of interruption to pin 2
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN),
                  buttonReleasedInterrupt,
                  FALLING);
//
Serial.begin(115200); // only to debug  
Serial.println("You reach a Reset Hardware!");               
}
void send_char_nBit(unsigned int a, unsigned int num)
{
 unsigned int data = 0x0000; //value to transmit in that case is 16bits
 unsigned int mask = 0x0001; //our bitmask
 unsigned int i = -1;
  data=a;
  for (mask = 0x0001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// 
    delayMicroseconds(1);
    if (i==num){
    Serial.println(); //Empty line
    Serial.print("Number of Bit as been sended: ");
    Serial.println(i); //Variable "i" represent a counter of number of bit's sent!
    delayMicroseconds(1);
    break;
    }  
  }
}
/*********************************************************************/
// I h've created 3 functions to send bit's, one with strobe, other without strobe and one with first byte with strobe followed by remaing bits.
void send_char_without(unsigned char a)
{
 unsigned char data = 170; //value to transmit, binary 10101010 to test
 unsigned char mask = 1; //our bitmask
  data=a;
      for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
      digitalWrite(VFD_clk, LOW);
      delayMicroseconds(1);
        if (data & mask){ // if bitwise AND resolves to true
          digitalWrite(VFD_in, HIGH);
          Serial.print(1);
        }
        else{ //if bitwise and resolves to false
          digitalWrite(VFD_in, LOW);
          Serial.print(0);
        }
        digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
        delayMicroseconds(1);
      }
  digitalWrite(VFD_clk, LOW);
  Serial.println();
}
/*********************************************************************/
void send_char_8bit_stb(unsigned char a)
{
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = -1;
  data=a;
  digitalWrite(VFD_ce, HIGH);
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// 
    delayMicroseconds(1);
    if (i==7){
    //Serial.println(i);
    digitalWrite(VFD_ce, LOW);
    delayMicroseconds(1);
    }  
  }
}
/*******************************************************************/
void allON(){
delayMicroseconds(1); 
//The burst of bit's have a size of 82Bit of segment and 3 bit of control (total of 85 bits)
send_char_without(0B11111111);  send_char_without(0B11111111); //  8:1   -16:9// First 14 segments display(Note: skip the 6 & 9)Belongs Second 
send_char_without(0B11111111);  send_char_without(0B11111111); // 24:17  -32:25// Second 14 segments display(Note: skip 25 & 26) Belongs Third 
send_char_without(0B11111111);  send_char_without(0B11111111); // 40:33  -48:41// Third 14 segments display(Note:Skip 40 & 44 the 41,42 & 43) Belongs first 7seg display 
send_char_without(0B11111111);  send_char_without(0B11111111);//  56:49  -69:62// //
send_char_without(0B11111111);  send_char_without(0B11111111); // 61:54  -69:62//
//send_char_nBit(nGrid, 5); //Here I'm sent the last 2 bits of 82 burst and add the 3 bit of selection of GRID
delayMicroseconds(1);  
}
void allOFF(){
delayMicroseconds(2);
send_char_without(0B00000000);  send_char_without(0B00000000); //  8:1   -16:9//  
send_char_without(0B00000000);  send_char_without(0B00000000); // 24:17  -32:25//  
send_char_without(0B00000000);  send_char_without(0B00000000); // 40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000);//  56:49   //
send_char_without(0B00000000);  send_char_without(0B00000000); // 61:54  -69:62//
//send_char_5bit(0x00); 
delayMicroseconds(2); 
}
void testGrid03(){
  digitalWrite(VFD_ce, HIGH); //
      allOFF();
      send_char_nBit((0x00), (0x05-1));
digitalWrite(VFD_ce, LOW); // 

digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B00001000);  send_char_without(0B10111010); //   8:1   -16:9//  
send_char_without(0B11100000);  send_char_without(0B10000000); //  24:17  -32:25//  
send_char_without(0B00111110);  send_char_without(0B00000100); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
}
/**************************************************************************/
void vumeter0(){
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 1
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00010000);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x01 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 2
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57// 
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x02 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
}
void vumeter1(){
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 1
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00000000);  send_char_without(0B00001000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x01 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 2
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57// 
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x02 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
}
void vumeter2(){
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 1
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00010000);  send_char_without(0B00111000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x01 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 2
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57// 
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x02 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
}
void vumeter3(){
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 1
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000011);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00010000);  send_char_without(0B00111000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x01 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 2
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57// 
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x02 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
}
void vumeter4(){
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 1
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00001111);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00010000);  send_char_without(0B00111000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x01 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 2
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57// 
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x02 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
}
void vumeter5(){
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 1
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00111111);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00010000);  send_char_without(0B00111000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x01 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 2
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B00000000); //  24:17  -32:25// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57// 
send_char_without(0B00000000);  send_char_without(0B00000000); //   8:1   -16:9//
send_char_nBit((0x02 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
}
void Hi_Folks(){
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
// Grid 3
send_char_without(0B10000000);  send_char_without(0B10000000); //   8:1   -16:9//  
send_char_without(0B01001011);  send_char_without(0B00001110); //  24:17  -32:25//  
send_char_without(0B11101000);  send_char_without(0B01000011); //  40:33  -48:41//  
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //  72:65  -80:73//
send_char_nBit(( 0x03 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 1
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B10000100); //   8:1   -16:9//
send_char_without(0B00000000);  send_char_without(0B01000000); //  24:17  -32:25// 
send_char_without(0B00000100);  send_char_without(0B00000000); //  40:33  -48:41// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  56:49  -64:57//
send_char_without(0B00000000);  send_char_without(0B00000000); //  72:65  -80:73//
send_char_nBit((0x01 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
// Grid 2
digitalWrite(VFD_ce, HIGH); //
delayMicroseconds(1);
send_char_without(0B00000000);  send_char_without(0B00000010); //   8:1   -16:9//
send_char_without(0B10001010);  send_char_without(0B00001100); //  24:17  -32:25// 
send_char_without(0B11100110);  send_char_without(0B00100001); //  40:33  -48:41// 
send_char_without(0B00000100);  send_char_without(0B00000000); //  56:49  -64:57// 
send_char_without(0B00000000);  send_char_without(0B00000000); //  72:65  -80:73//
send_char_nBit((0x02 << 2), (0x05-1));
digitalWrite(VFD_ce, LOW); //
delayMicroseconds(1);
}
/****************************************************************************/
void searchOfSegments(){
// put your main code here, to run repeatedly:
  int group = 0x00;
  byte nBit =0x00;
  byte nMask = 0b00000001;
  unsigned int block =0;
  unsigned int nSeg=0x00;  
  Serial.println();
  Serial.println("We start the test of segments!");
    for( group=0; group<11; group++){   // Do until n bits 7*8 bits
        //for(int nBit=0; nBit<8; nBit++){
          for (nMask = 0b00000001; nMask>0; nMask <<= 1){
            Aa=0x00; Ab=0x00; Ac=0x00; Ad=0x00; Ae=0x00; Af=0x00; Ag=0x00; Ah=0x00; Ai=0x00; Aj=0x00;
                  switch (group){
                    case 0: Aa=nMask; break;
                    case 1: Ab=nMask; break;
                    case 2: Ac=nMask; break;
                    case 3: Ad=nMask; break;
                    case 4: Ae=nMask; break;
                    case 5: Af=nMask; break;
                    case 6: Ag=nMask; break;
                    case 7: Ah=nMask; break;
                    case 8: Ai=nMask; break;
                    case 9: Aj=nMask; break;
                  }
            
           nSeg++;
          
      //This start the control of button to allow continue teste! 
                      while(1){
                            if(!buttonReleased){
                              delay(200);
                            }
                            else{
                              delay(15);
                               buttonReleased = false;
                               break;
                               }
                         }
               delay(50);
              segments();
            Serial.print(nSeg, DEC); Serial.print(", group: "); Serial.print(group, DEC);Serial.print(", BlockBit: "); Serial.print(blockBit, HEX);Serial.print(", nMask: "); Serial.print(nMask, BIN);Serial.print("   \t");
            Serial.print(Aj, HEX);Serial.print(", ");Serial.print(Ai, HEX);Serial.print(", ");
            Serial.print(Ah, HEX);Serial.print("; ");Serial.print(Ag, HEX);Serial.print(", ");Serial.print(Af, HEX);Serial.print(", ");Serial.print(Ae, HEX);Serial.print(", ");
            Serial.print(Ad, HEX);Serial.print(", ");Serial.print(Ac, HEX);Serial.print(", ");Serial.print(Ab, HEX);Serial.print(", ");Serial.print(Aa, HEX); Serial.print("; ");
            
            Serial.println();
            delay (250);           
           }        
      }
  }
void segments(){
//Bit function: 
      digitalWrite(VFD_ce, HIGH); //
      delayMicroseconds(1);
      // 
          send_char_without(Aa);  send_char_without(Ab);  //   8:1     -16:9// 
          send_char_without(Ac);  send_char_without(Ad);  //  24:17    -32:25//
          send_char_without(Ae);  send_char_without(Af);  //  40:33    -48:41// 
          send_char_without(Ag);  send_char_without(Ah);  //  52:49   //
          send_char_without(Ai);  send_char_without(Aj);  //  52:49   //
          send_char_nBit(0x00, 0x04); //This write to all grid's and complet the bit 82 more 3 bits control!!!
      //    
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); //                   
}
//
void brightHIGH(){
  digitalWrite(VFD_ce, HIGH);      
  send_char_nBit(0x13FF, 0x0C); //This is the command of dimmer! 10bits of dimmer adjust and 3 bits of control. This result at 13(C) bits! 
  digitalWrite(VFD_ce, LOW);
}
void brightLOW(){
  digitalWrite(VFD_ce, HIGH);      
  send_char_nBit(0x1000, 0x0C); //This is the command of dimmer! 10bits of dimmer adjust and 3 bits of control. This result at 13(C) bits! 
  digitalWrite(VFD_ce, LOW);
}
void vumeter(){
  for (unsigned int c=0; c<6;c++){
    vumeter0();
delay(1000);
vumeter1();
delay(1000);
vumeter2();
delay(1000);
vumeter3();
delay(500);
vumeter4();
delay(500);
vumeter5();
delay(300);
  }
}
void dimmerUp(){
  for (unsigned int t = 0x00; t<0x03FF; t++){
  digitalWrite(VFD_ce, HIGH);      
  send_char_nBit((0x1000 | t), 0x0C); //This is the command of dimmer! 10bits of dimmer adjust and 3 bits of control. This result at 13(C) bits! 
  digitalWrite(VFD_ce, LOW);
  delay(5);
  }
//  digitalWrite(VFD_ce, HIGH);      
//  send_char_nBit(0x13FF, 0x0C); //This is the command of dimmer! 10bits of dimmer adjust and 3 bits of control. This result at 13(C) bits! 
//  digitalWrite(VFD_ce, LOW);
}
void dimmerDown(){

  for (unsigned int t = 0x03FF; t>1; t--){
  digitalWrite(VFD_ce, HIGH);      
  send_char_nBit((0x1000 | t), 0x0C); //This is the command of dimmer! 10bits of dimmer adjust and 3 bits of control. This result at 13(C) bits! 
  digitalWrite(VFD_ce, LOW);
  delay(5);
  }
//  digitalWrite(VFD_ce, HIGH);      
//  send_char_nBit(0x13FF, 0x0C); //This is the command of dimmer! 10bits of dimmer adjust and 3 bits of control. This result at 13(C) bits! 
//  digitalWrite(VFD_ce, LOW);
}
void loop() {
  brightHIGH(); //Here we call the 3 lines to put the bright HIGH, by default it stay a low value!!!
  //
  vumeter();
  //
    for(int s=0; s<4; s++){
      digitalWrite(VFD_ce, HIGH); //
      allON();
      gridMore2bits = (s << 2);
      send_char_nBit((gridMore2bits | 0x03), (0x05-1));
      digitalWrite(VFD_ce, LOW); // 
      delay(1000);
      digitalWrite(VFD_ce, HIGH); //
      allOFF();
      gridMore2bits = (s << 2);
      send_char_nBit((gridMore2bits | 0x03), (0x05-1));
      digitalWrite(VFD_ce, LOW); //
      delay(500);
    }
delay(2000);
//
     for(int s=0; s<4; s++){
      digitalWrite(VFD_ce, HIGH); //
      allOFF();
      gridMore2bits = (s << 2);
      send_char_nBit((gridMore2bits | 0x03), (0x05-1));
      digitalWrite(VFD_ce, LOW); // 
      delay(1000);
      digitalWrite(VFD_ce, HIGH); //
      allON();
      gridMore2bits = (s << 2);
      send_char_nBit((gridMore2bits | 0x03), (0x05-1));
      digitalWrite(VFD_ce, LOW); //
      delay(500);
    }  
//
digitalWrite(VFD_ce, HIGH); //
      allOFF();
      send_char_nBit((0x00), (0x05-1));
digitalWrite(VFD_ce, LOW); //
//
brightLOW();
//
Hi_Folks();
dimmerUp();
         for(unsigned int v=0; v<6; v++){
          digitalWrite(VFD_ce, HIGH); //
              allOFF();
              send_char_nBit((0x00), (0x05-1));
        digitalWrite(VFD_ce, LOW); // 
        delay(200);
        Hi_Folks();
        delay(400);
         }
dimmerDown();
delay(250);
dimmerUp();
//
searchOfSegments(); //Uncomment this line if you want run the find segments 
}
void buttonReleasedInterrupt() {
  buttonReleased = true; // This is the line of interrupt button to advance one step on the search of segments!
}
