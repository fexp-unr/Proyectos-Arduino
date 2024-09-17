/ constants won't change
const int BUTTON_PIN = 7;  // Connect the Button to pin 7 or change here
const int LED_PIN    = 3;       // Connect the LED to pin 3 or change here

cons int ENCODER_PIN = 2;  // The pin the encoder is connected           
unsigned int rpm;     // rpm reading
volatile byte pulses;  // number of pulses
unsigned long timeold; 
// The number of pulses per revolution: depends on your index disc!!
unsigned int pulsesperturn = 1;

// variables will change:
int ledState = LOW;        // tracks the current state of LED
int measuringState = LOW;
int lastButtonState;        // the previous state of button
int currentButtonState; // the current state of button

void counter()
 {
    //Update count
      pulses++;    
 }

void setup() {
  // begin serial port
  Serial.begin(9600);
  
  //Use statusPin to flash along with interrupts
   pinMode(encoder_pin, INPUT);
   
   //Interrupt 0 is digital pin 2, so that is where the IR detector is connected
   //Triggers on FALLING (change from HIGH to LOW)
   attachInterrupt(0, counter, FALLING);
   // Initialize
   pulses = 0;
   rpm = 0;
   timeold = 0;
  // set arduino pin to input mode
  pinMode(BUTTON_PIN, INPUT);
  // set arduino pin to output mode
  pinMode(LED_PIN, OUTPUT);
  currentButtonState = digitalRead(BUTTON_PIN);
}

void measure(const int TRIG_PIN, const int ECHO_PIN) {



 void loop()
 {
   if (millis() - timeold >= 1000){  /*Uptade every one second, this will be equal to reading frecuency (Hz).*/
 
  //Don't process interrupts during calculations
   detachInterrupt(0);
   //Note that this would be 60*1000/(millis() - timeold)*pulses if the interrupt
   //happened once per revolution
   rpm = (60 * 1000 / pulsesperturn )/ (millis() - timeold)* pulses;
   timeold = millis();
   pulses = 0;
   
   //Write it out to serial port
   Serial.print("RPM = ");
   Serial.println(rpm,DEC);
   //Restart the interrupt processing
   attachInterrupt(0, counter, FALLING);
   }
  }
}
  
void loop() {
  lastButtonState    = currentButtonState;                // save the last state
  currentButtonState = digitalRead(BUTTON_PIN); // read new state
  
  if(lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.print("The button is pressed: ");
  
    // toggle state of LED
    if(ledState == LOW) {
       ledState = HIGH;
       measuringState = HIGH;
       Serial.println("Iniciando medición");
       measure(TRIG_PIN, ECHO_PIN);
    }
    else {
      ledState = LOW;
      measuringState = LOW;
      Serial.println("Interrupiendo medición");
    }
 
    // control LED arccoding to the toggled state
    digitalWrite(LED_PIN, ledState);  //turns the LED on or off based on the variable
  }else{
    if(measuringState == HIGH){
      measure(TRIG_PIN, ECHO_PIN);
    }
  }
  delay(100);
}
