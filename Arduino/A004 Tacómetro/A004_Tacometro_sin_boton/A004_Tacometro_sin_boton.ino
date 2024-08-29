const unsigned int TIEMPO_MUERTO = 1000; //tiempo entre mediciones
const int INTERRUPT_PIN = 2;  // El pin al que se conecta el sensor.
unsigned int omega;     // la velocidad angular del molino
volatile byte pulsos;  // número de pulsos. El tipo volatile le dice al programa que esta variable puede cambiar su valor en cualquier momento
unsigned long tiempo_inicial; // tiempo en que inicia la medición
unsigned int pulsosporvuelta = 1; // El número de pulsos que se espera contar durante una vuelta completa del molino

void contador() {
   pulsos++;   
}

void setup() {
Serial.begin(9600);  // Inicio la comunicación con la PC
 pinMode(INTERRUPT_PIN, INPUT); //Defino al pin 2 como entrada
 attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), contador, FALLING);  //Se asocia a la función counter a las interrupciones del pin 2. Se considera una interrupción a la baja de la señal (FALLING, es decir el cambio de HIGH to LOW)
 pulsos = 0; // Inicializo las variables en 0
 tiempo_inicial = 0;
}

void loop() {
 if (millis() - tiempo_inicial >= TIEMPO_MUERTO){ /*Una vez por segundo hago la cuenta de la velocidad angular del molino. */
   detachInterrupt(0); //No cuento los pulsos mientras cuento las vueltas
   omega = pulsos / pulsosporvuelta; //Divido la cantidad de pulsos por 4 para determinar la velocidad angular en vueltas por segundo
   tiempo_inicial = millis(); //Redefino el tiempo inicial de la medición
   pulsos = 0; //Vuelvo a cero el conteo de pulsos
   Serial.print("RPM = ");
   Serial.println(omega,DEC);  //Escribo la salida en pantalla
  attachInterrupt(0, contador, FALLING); //Vuelvo a iniciar el conteo de pulsos
 }
}