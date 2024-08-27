const int inputPin = 2;
volatile unsigned long lowTime = 0;

void setup () {
  pinMode ( inputPin , INPUT ) ;
  Serial . begin (9600) ;
  TCCR2A = 0;
  TCCR2B = (1 << CS20 ) ; // Habilito usar el registro completo de tiempos del clock del arduino
  TIMSK2 = (1 << TOIE2 ) ; //Habilito el uso de las interrupciones de clock del Timer2 del arduino
  sei () ;
}

void loop () {

}

ISR ( TIMER2_OVF_vect ) { // dispara cuando el clock del timer2 hace overflow, aproximadamente una vez por ms, para arduino UNO
  static unsigned long startTime = 0;
  if ( digitalRead ( inputPin ) == LOW ) {
    if ( startTime == 0) {
      startTime = micros () ;
    }
  } else if ( startTime != 0) {
    lowTime = micros () - startTime ;
    startTime = 0;
    Serial . print (" Tiempo en LOW : ") ;
    Serial . print ( lowTime ) ;
    Serial . println (" microsegundos ") ;
  }
}
