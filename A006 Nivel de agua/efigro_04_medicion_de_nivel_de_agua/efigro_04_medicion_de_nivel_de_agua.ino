/*-------INICIO DEL CÓDIGO, COPIAR DESDE ESTA PARTE-------*/
// Parte 1: definiciones generales de las variables y constantes que se van a utilizar en el código
const int POWER_PIN = 7; //Le indico al programa que el pin 7 se va a usar para suministrar energía
const int SIGNAL_PIN = A0; //Le indico al programa que el pin A0 se va a usar para leer la señal del sensor
int nivel = 0; // variable que va a registrar el nivel de agua


// Parte 2: la función setup es una función que no tiene parámetros de entrada y que no devuelve ningún valor. Es una de las dos funciones que obligadamente aparecen en cualquier programa Arduino. En esta función inicializamos el hardware y su conexión con la PC. Es una función que correrá una sola vez al inicio del programa
void setup() {
 Serial.begin(9600); // Se inicia la comunicación con el monitor de la PC a una velocidad de 9600 bits por segundo (baudios)
 pinMode(POWER_PIN, OUTPUT);   // configuro el pin 7 como salida (de energía eléctrica)
 digitalWrite(POWER_PIN, LOW); // me aseguro de que al inicio el pin de salida esté apagado (LOW = Apagado)
}


// Parte 3: la función loop es la segunda función obligatoria de un programa Arduino. Al igual que la función setup no tiene parámetros de entrada y que no devuelve ningún valor. Una vez que el programa esté corriendo esta función se ejecutará una y otra vez mientras la placa tenga alimentación eléctrica
void loop() {
 digitalWrite(POWER_PIN, HIGH);  // le doy energía al sensor (HIGH = Encendido)
 delay(10);                      // le digo al programa que espere 10 milisegundos
 nivel = analogRead(SIGNAL_PIN); // registro la lectura del sensor
 digitalWrite(POWER_PIN, LOW);   // le quito la energía al sensor


 Serial.print("Nivel de agua: "); // escribo en pantalla la lectura el texto "Nivel de agua: "
 Serial.println(nivel); // escribo en pantalla la lectura del sensor

 delay(2000); // espero 1000 milisegundos (1 s) antes de volver a correr el código
}
/*-------FIN DEL CÓDIGO-------*/