void setup() {
  Serial.begin(9600); // Inicializa el puerto serie a 9600 baudios
}

void loop() {
  if (Serial.available() > 0) { // Si hay datos disponibles en el puerto serie
    char incomingByte = Serial.read(); // Lee el byte entrante
    Serial.print("Recibido: ");
    Serial.println(incomingByte); // Imprime el byte recibido
  }
}
