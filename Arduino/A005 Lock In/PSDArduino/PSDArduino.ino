/* {PSDARduino
    Copyright (C) 2014  Kevin D. Schultz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

/* This program is used to configure the Arduino as a function
  generator and as DAQ. This sketch can be used in just this capacity,
  but is intended to be the reference signal generator and the front
  end of a Phase-Sensitive-Detector with the aid of Processing program
  2Ch-PSD.pde

  The reference signal is generated by modifiying the PWM on pin 9
  of an Arduino.This signal is smoothed with an RC ckt on the pin 9.
  A NPN transistor is used afterwards to drive an LED. The signal
  from a LED acting as a photodiode is read in from pin 0.

  To make this all work quickly, although it is not strictly necessary
  We run the ADC in it's fastest mode see, https://sites.google.com/site/measuringstuff/the-arduino

  See Fritzing Sketch PSD_sketch.fzz

*/

//Modified by FLE 20220130
// Download from:
//Add sync pin 8 output to trigger Oscilliscope on oscillator output
//Change variable types for sin/cos function table.
// 20220131 /Move sine and cosine into PROGMEM
// 20220201 Clean up comments
// Mul by 7.8125 to make mV.
// Add a syncQ output on pin 7.

#include <avr/interrupt.h>

#define PI2 6.283185
const int TABLE_OFFSET = 128;

//const int inputOFFSET = 1024/2;       //Theoritical input mid point.
const int inputOFFSET = 2.39 * 1023 / 4.80; //Measured on Resistive devider input FLE 20220201

#define REC_LENGTH 256                //Input signal array length
#define LENGTH 256                    //Sine Cosine table array length

const int syncPin = 8;  //Inphase sync
const int syncQPin = 7;  //Qphase sync

//const int myINPUT = 0;
const int myINPUT = A2;    //OctoUNO J1Pin1 input wire White/Orange

//ToDo make the sine and cosine 10bit resolution. Remove offset from table.
//Sine and cosine table of length = LENGTH = 256. Sine and cosine offset by 128. Range is 0-256.
const static int16_t PROGMEM wave_I[] = {
  255, 254, 254, 254, 254, 254, 253, 253, 252, 251, 251, 250, 249, 248, 247, 246, 245, 244, 242, 241, 240, 238, 236, 235, 233, 231, 230, 228, 226, 224, 222, 219, 217, 215, 213, 210, 208, 206, 203, 201, 198, 195, 193, 190, 187, 185, 182, 179, 176, 173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 76, 73, 70, 68, 65, 62, 60, 57, 54, 52, 49, 47, 45, 42, 40, 38, 36, 33, 31, 29, 27, 25, 24, 22, 20, 19, 17, 15, 14, 13, 11, 10, 9, 8, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 17, 19, 20, 22, 24, 25, 27, 29, 31, 33, 36, 38, 40, 42, 45, 47, 49, 52, 54, 57, 60, 62, 65, 68, 70, 73, 76, 79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 115, 118, 121, 124, 128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173, 176, 179, 182, 185, 187, 190, 193, 195, 198, 201, 203, 206, 208, 210, 213, 215, 217, 219, 222, 224, 226, 228, 230, 231, 233, 235, 236, 238, 240, 241, 242, 244, 245, 246, 247, 248, 249, 250, 251, 251, 252, 253, 253, 254, 254, 254, 254, 254
};

const static int16_t PROGMEM wave_Q[] = {
  128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 161, 164, 167, 170, 173, 176, 179, 182, 185, 187, 190, 193, 195, 198, 201, 203, 206, 208, 210, 213, 215, 217, 219, 222, 224, 226, 228, 230, 231, 233, 235, 236, 238, 240, 241, 242, 244, 245, 246, 247, 248, 249, 250, 251, 251, 252, 253, 253, 254, 254, 254, 254, 254, 255, 254, 254, 254, 254, 254, 253, 253, 252, 251, 251, 250, 249, 248, 247, 246, 245, 244, 242, 241, 240, 238, 236, 235, 233, 231, 230, 228, 226, 224, 222, 219, 217, 215, 213, 210, 208, 206, 203, 201, 198, 195, 193, 190, 187, 185, 182, 179, 176, 173, 170, 167, 164, 161, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 76, 73, 70, 68, 65, 62, 60, 57, 54, 52, 49, 47, 45, 42, 40, 38, 36, 33, 31, 29, 27, 25, 24, 22, 20, 19, 17, 15, 14, 13, 11, 10, 9, 8, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 17, 19, 20, 22, 24, 25, 27, 29, 31, 33, 36, 38, 40, 42, 45, 47, 49, 52, 54, 57, 60, 62, 65, 68, 70, 73, 76, 79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 115, 118, 121, 124,
};

volatile byte index = 0; // Points to each table entry. declare as volatile so other functions can see it.

int signal[REC_LENGTH]; // Captures Signal Data into array

/* We are going to reconfigure ADC to be faster to go to regular speed
  change to #define FASTADC 0 */
#define FASTADC 1
// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); ////FLE Make Faster
  delay(100);
  Serial.println("ImV, QmV");       //Set scale for serial ploter
  pinMode(syncPin, OUTPUT);         //Setup a pin for sync signal output
  digitalWrite(syncPin, HIGH);
  pinMode(syncQPin, OUTPUT);         //Setup a pin for sync Q signal output
  digitalWrite(syncQPin, HIGH);

#if FASTADC
  // set prescale to 16
  sbi(ADCSRA, ADPS2) ;
  cbi(ADCSRA, ADPS1) ;
  cbi(ADCSRA, ADPS0) ;
#endif

//  Serial.println(sizeof(wave_I) / sizeof(wave_I[0]));

  pinMode(9, OUTPUT); //this is going to be where PWM signal comes out. Then goes to LPF.
  TCCR1B = (1 << CS10); //Timer runs at 16MHz
  TCCR1A |= (1 << COM1A1); //Sets pin low when TCNT1 = OCR1A
  TCCR1A |= (1 << WGM10); // This and next line sets 8-bit fast PWM mode
  TCCR1B |= (1 << WGM12);

  /*Get timer 2 ready to call interupt function*/
  TCCR2A = 0; //Control register A for timer 2
  TCCR2B = (1 << CS21); //Slow this one down so prescaler is divided by factor of 8
  TIMSK2 = (1 << OCIE2A); //When TCNT2=OCRA2 call the ISR

  //  OCR2A = 50; // Sets frequency  ???
  OCR2A = 450; // Sets frequency  34.7 Hz
  //  OCR2A = 511; // Sets frequency  27.67 Hz

  sei();//enable interupts
  establishContact();             //Wait for serial command from controller.
  digitalWrite(syncPin, LOW);
  digitalWrite(syncQPin, LOW);
}// end setup()

void loop() {
  /* After we have filled up the array signal from the ADC[], send array
    over to Processing via serial. Only the mixed I/Q data is sent
    filtering and display are to be done in processing.

    20220201
    Signal in at VCC/2 makes I and q = 0.
    signal in at VCC  makes I and q = +/- 313.
    signal in at GND  makes I and q = +/- 313.
    mixI and mixQ = 128
    2.5*128 = 320
    Mul by 0.0078125 to make volts.
    Mul by 7.8125 to make mV.
  */

  if ((index >= LENGTH - 1) & (Serial.available() > 0)) // We have a filled array and S port available.
  {
    int hold;
    for (int i = 0; i < LENGTH; i++)
    {
      //Serial.print(wave_I[i]);
      //Serial.print(',');
      hold = ((signal[i] - inputOFFSET) * 5.0 / 1023.0) * 7.8125 * (int(pgm_read_word_near(wave_I + i)) - 1 * TABLE_OFFSET) ;
      Serial.print(hold);//send in-phase mix
      Serial.print(',');
      hold = ((signal[i] - inputOFFSET) * 5.0 / 1023.0) * 7.8125 * (int(pgm_read_word_near(wave_Q + i)) - 1 * TABLE_OFFSET) ;
      Serial.println(hold);//send out-of-phase mix
    }// end FOR
  }
}// end looop()

ISR(TIMER2_COMPA_vect) { // Called when TCNT2 == OCR2A
  signal[index] = analogRead(myINPUT);
  OCR1AL = wave_I[index++]; // Update the PWM output
  //asm(“NOP;NOP”); // Fine tuning
  TCNT2 = 12; // Timing to compensate for ISR run time
  //Sync signal for oscilliscope. This will have zero phase delay (otherwise due to continious time LPF).
  if (index == 0) { 
    digitalWrite(syncPin, HIGH);
  }
  if (index ==   LENGTH /4) {
    digitalWrite(syncQPin, HIGH);
  }
  if (index ==   LENGTH / 2) {
    digitalWrite(syncPin, LOW);
  }
  if (index ==   LENGTH * 3/4) {
    digitalWrite(syncQPin, LOW);
  }
}//end ISR

//Wait for any character from controller to start data
void establishContact() {
  while (Serial.available() <= 0) {
    delay(300);
    Serial.println("2.5,0");   // send an initial string    
  }
}// end establishContact()