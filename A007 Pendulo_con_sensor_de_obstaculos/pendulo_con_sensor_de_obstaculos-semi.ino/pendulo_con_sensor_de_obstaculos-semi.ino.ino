/*-------INICIO DEL CÓDIGO-------*/
// Parte 1: definiciones generales de las variables y constantes que se van a utilizar en el código
const int inputPin = 2; // defino el Pin del que voy a leer la señal como el 2
unsigned long tiempoHIGH = 0; // defino la variable que va a registrar el tiempo en que sensor esté frente a un obstáculo
static unsigned long tiempoInicial = 0; // defino la variable que va a guardar el tiempo inicial
/* Parte 2: la función setup es una función que no tiene parámetros de entrada y que no devuelve ningún valor.
Es una de las dos funciones que obligadamente aparecen en cualquier programa Arduino. En esta función inicializamos el hardware y su conexión con la PC. Es una función que correrá una sola vez al inicio del programa */
void setup () {
 pinMode(inputPin, INPUT); // le digo a la placa que el pin 2 va a ser un pin que reciba información
 Serial.begin (9600);
}
/* Parte 3: la función loop es la segunda función obligatoria de un programa Arduino. Al igual que la función setup no tiene parámetros de entrada y que no devuelve ningún valor. Una vez que el programa esté corriendo esta función se ejecutará una y otra vez mientras la placa tenga alimentación eléctrica */
void loop () {
 if (digitalRead(inputPin) == HIGH){ //si el pin da una señal HIGH... (es decir, tiene un obstáculo enfrente)
     if (tiempoInicial == 0){ // ...y si el tiempo inicial es 0...
       tiempoInicial = millis(); // ...entonces empiezo a medir el tiempo
     }
 }else if (tiempoInicial != 0){ // si llegamos a esta parte del código con tiempoInicial diferente de 0, es porque hubo un objeto frente al sensor, que ya se fue del mismo
    tiempoHIGH = millis() - tiempoInicial; //mido el tiempo que pasó entre el tiempo inicial y el momento actual (medio periodo)
    tiempoInicial = 0; // reseteo el tiempo inicial
    Serial.print("Tiempo de bloqueo: "); // doy salida de pantalla al tiempo medido
    Serial.print(tiempoHIGH);
    Serial.println(" ms");
 }
 delay(1); // espero un milisegundo para volver a medir
}
/*-------FIN DEL CÓDIGO-------*/