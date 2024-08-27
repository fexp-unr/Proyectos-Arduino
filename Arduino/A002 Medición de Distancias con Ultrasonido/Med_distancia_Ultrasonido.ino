const int trigPin = 9;
const int echoPin = 8;
float speed_sound = 0.0343; // velocidad en cm/micro_s
float duration;
float distance;
float t0, t;

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication
  t0 = millis();
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * speed_sound / 2;
  t = millis() - t0;
  // Prints the distance on the Serial Monitor
  Serial.print(t);
  Serial.print(" ");
  Serial.println(distance);
  delay(1000);
}
