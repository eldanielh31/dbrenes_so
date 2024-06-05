#include <Servo.h>

//Variables 
int posInicio_servoAdelanteAtras = 110;
int posInicio_servoArribaAbajo = 158;
int posInicio_servoDerechaIzquierda = 85;

int led_Espera = 4;
int led_Ejecucion = 5;

// Servos
Servo servo_DerechaIzquierda;
Servo servo_AdelanteAtras;
Servo servo_ArribaAbajo;

void setup() {
  Serial.begin(9600);
 
  // Definicion de pines
  servo_DerechaIzquierda.attach(6);
  servo_ArribaAbajo.attach(9);
  servo_AdelanteAtras.attach(10);

  pinMode(led_Espera, OUTPUT);
  pinMode(led_Ejecucion, OUTPUT);

  servo_ArribaAbajo.write(150);
}
 
void loop() {
  if (Serial.available() > 0) {
    String palabra = Serial.readStringUntil('\n');  // Leer la cadena hasta un salto de línea
    palabra.trim(); // Eliminar cualquier espacio en blanco
    palabra.toLowerCase(); // Convertir la palabra a minúsculas

    mover_Inicio();

    // Ejecutar la acción basada en cada letra de la palabra ingresada
    for (int i = 0; i < palabra.length(); i++) {
      char letra = palabra.charAt(i);
      ejecutarAccionPorLetra(letra);
    }
  }

  encender_ledEspera();
}

void encender_ledEspera(){
  digitalWrite(led_Espera, HIGH);
  digitalWrite(led_Ejecucion, HIGH); delay(400);
  digitalWrite(led_Espera, LOW);
  digitalWrite(led_Ejecucion, LOW); delay(400); 
}

void encender_ledEjecucion(){
  digitalWrite(led_Espera, HIGH); delay(50);
  digitalWrite(led_Espera, LOW); delay(50);
  digitalWrite(led_Ejecucion, HIGH); delay(50);
  digitalWrite(led_Ejecucion, LOW); delay(50); 
}

void mover_Inicio(){
  // VOLVER A POSICION DE INICIO
  servo_DerechaIzquierda.write(posInicio_servoDerechaIzquierda);  delay(250);
  servo_ArribaAbajo.write(posInicio_servoArribaAbajo);  delay(250);
  servo_AdelanteAtras.write(posInicio_servoAdelanteAtras);  delay(250);
}

void mover_AdelanteAtras(int angulo){
  /* SERVO ADELANTE - ATRAS
      Adelante => angulo cercano a 180 
      Atras    => angulo cercano a 0 
 */
 encender_ledEjecucion();
  servo_AdelanteAtras.write(angulo);
  delay(450);
  encender_ledEjecucion();
}

void mover_IzquierdaDerecha(int angulo){
  /* SERVO IZQUIERDA - DERECHA
      Izquierda => angulo cercano a 180 
      Derecha   => angulo cercano a 0 
  */
  encender_ledEjecucion();
  servo_DerechaIzquierda.write(angulo);
  delay(450);
  encender_ledEjecucion();
}

void mover_ArribaAbajo(int abajo, int arriba) {
  /* SERVO ARRIBA - ABAJO
      Abajo  => angulo cercano a 0 
      Arriba => angulo cercano a 180 
  */
  encender_ledEjecucion();
  servo_ArribaAbajo.write(abajo);
  delay(450);
  encender_ledEjecucion();
  servo_ArribaAbajo.write(arriba);
  delay(450);
  encender_ledEjecucion();
}

void ejecutarAccionPorLetra(char letra) {
  switch (letra) {
    case 'a': letraA(); break;
    case 'b': letraB(); break;
    case 'c': letraC(); break;
    case 'd': letraD(); break;
    case 'e': letraE(); break;
    case 'f': letraF(); break;
    case 'g': letraG(); break;
    case 'h': letraH(); break;
    case 'i': letraI(); break;
    case 'j': letraJ(); break;
    case 'k': letraK(); break;
    case 'l': letraL(); break;
    case 'm': letraM(); break;
    case 'n': letraN(); break;
    case 'o': letraO(); break;
    case 'p': letraP(); break;
    case 'q': letraQ(); break;
    case 'r': letraR(); break;
    case 's': letraS(); break;
    case 't': letraT(); break;
    case 'u': letraU(); break;
    case 'v': letraV(); break;
    case 'w': letraW(); break;
    case 'x': letraX(); break;
    case 'y': letraY(); break;
    case 'z': letraZ(); break;
    default:
      mover_Inicio();
      break;
  }
}

// PRESIONAR LETRA A
void letraA(){
  mover_IzquierdaDerecha(123);
  mover_AdelanteAtras(128);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA B
void letraB(){
  mover_IzquierdaDerecha(84);
  mover_AdelanteAtras(91);
  mover_ArribaAbajo(116, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA C
void letraC(){
  mover_IzquierdaDerecha(105);
  mover_AdelanteAtras(98);
  mover_ArribaAbajo(116, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA D
void letraD(){
  mover_IzquierdaDerecha(106);
  mover_AdelanteAtras(111);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA E
void letraE(){
  mover_IzquierdaDerecha(105);
  mover_AdelanteAtras(130);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA F
void letraF(){
  mover_IzquierdaDerecha(99);
  mover_AdelanteAtras(107);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA G
void letraG(){
  mover_IzquierdaDerecha(90);
  mover_AdelanteAtras(103);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA H
void letraH(){
  mover_IzquierdaDerecha(80);
  mover_AdelanteAtras(106);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA I
void letraI(){
  mover_IzquierdaDerecha(66);
  mover_AdelanteAtras(130);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA J
void letraJ(){
  mover_IzquierdaDerecha(70);
  mover_AdelanteAtras(110);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA K
void letraK(){
  mover_IzquierdaDerecha(62);
  mover_AdelanteAtras(115);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA L
void letraL(){
  mover_IzquierdaDerecha(55);
  mover_AdelanteAtras(122);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA M
void letraM(){
  mover_IzquierdaDerecha(63);
  mover_AdelanteAtras(102);
  mover_ArribaAbajo(116, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA N
void letraN(){
  mover_IzquierdaDerecha(74);
  mover_AdelanteAtras(96);
  mover_ArribaAbajo(116, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA O
void letraO(){
  mover_IzquierdaDerecha(60);
  mover_AdelanteAtras(134);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA P
void letraP(){
  mover_IzquierdaDerecha(52);
  mover_AdelanteAtras(137);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA Q
void letraQ(){
  mover_IzquierdaDerecha(120);
  mover_AdelanteAtras(137);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA R
void letraR(){
  mover_IzquierdaDerecha(99);
  mover_AdelanteAtras(126);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA S
void letraS(){
  mover_IzquierdaDerecha(116);
  mover_AdelanteAtras(120);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA T
void letraT(){
  mover_IzquierdaDerecha(90);
  mover_AdelanteAtras(122);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA U
void letraU(){
  mover_IzquierdaDerecha(73);
  mover_AdelanteAtras(125);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA V
void letraV(){
  mover_IzquierdaDerecha(96);
  mover_AdelanteAtras(93);
  mover_ArribaAbajo(116, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA W
void letraW(){
  mover_IzquierdaDerecha(114);
  mover_AdelanteAtras(133);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA X
void letraX(){
  mover_IzquierdaDerecha(116);
  mover_AdelanteAtras(108);
  mover_ArribaAbajo(116, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA Y
void letraY(){
  mover_IzquierdaDerecha(82);
  mover_AdelanteAtras(122);
  mover_ArribaAbajo(120, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}

// PRESIONAR LETRA Z
void letraZ(){
  mover_IzquierdaDerecha(123);
  mover_AdelanteAtras(112);
  mover_ArribaAbajo(116, 170);
  mover_AdelanteAtras(posInicio_servoAdelanteAtras);
  delay(200);
  mover_Inicio();
}
