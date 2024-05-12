// Definimos el pin del LED
const int ledPin = 8;

void setup() {
  // Inicializamos el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  
  // Inicializamos la comunicación serial a 9600 baudios
  Serial.begin(9600);
}

void loop() {
  // Verificamos si se ha presionado una tecla en el puerto serie
  if (Serial.available() > 0) {
    // Leemos la tecla presionada
    char tecla = Serial.read();
    
    // Verificamos si la tecla presionada es 'a'
    if (tecla == 'a') {
      // Encendemos el LED
      digitalWrite(ledPin, HIGH);
    }
    
    // Verificamos si la tecla presionada es 's'
    if (tecla == 's') {
      // Apagamos el LED
      digitalWrite(ledPin, LOW);
    }
  }
}
