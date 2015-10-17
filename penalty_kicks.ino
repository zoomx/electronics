/*
-------------------
copyright : (C) 2013 by Germán Carrillo
email : geotux_tuxman@linuxmail.org
----------------------------------------------------------------------------
----------------------------------------------------------------------------
* *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or *
* (at your option) any later version. *
* *
----------------------------------------------------------------------------
*/

/*
  Programa para simular movimientos de un arquero en lanzamientos de tiro penal
  En total hay 10 movimientos, dependiendo de la zona del arco que se quiera cubrir.
  
  Dichos 10 movimientos del arquero corresponden a combinaciones de 3 movimientos 
  independientes controlados a traves de Arduino: Un movimiento de rotacion del 
  arquero sobre su cadera, un movimiento de traslacion horizontal para dirigirse 
  hacia los palos, y un movimiento vertical para simular el salto.
*/
#include <Servo.h>  // servo library


/*      C     O     N     T     R     O     L       */

int SER_Pin = 12;   //pin 14 on the 75HC595  8
int RCLK_Pin = 8;  //pin 12 on the 75HC595  9
int SRCLK_Pin = 7; //pin 11 on the 75HC595 10

#define number_of_74hc595s 2 //How many of the shift registers 
#define numOfRegisterPins number_of_74hc595s * 8 //do not touch

boolean registers[numOfRegisterPins];

// Pines Arduino para los grupos de botones: A1 <-> Grupo 1, A2 <-> Grupo 2
const int pinBotones1 = 1;     
const int pinBotones2 = 2;     

  /* Pos _______________           LEDs   _______________           Grupos _______________           
        | A  B  C  D  E |                | 6 10  5  3  1 |                | 2  2  2  1  1 |          
        | F  G  H  I  J |                | 7 11 12  4  2 |                | 2  2  2  1  1 |
    =========================        =========================        =========================  
           R        Y                       9                                1        1
  */
char posiciones[12] = {'A','B','C','D','E','F','G','H','I','J','R','Y'};
char LEDs[11] = {6,10,5,3,1,7,11,12,4,2,9};

// Limite para considerar que no hay boton presionado
const int LIMITE_LECTURA_NULA = 400;

// Grupo de botones del pin A1
const int BOTON1_1LOW = 521;
const int BOTON1_1HIGH = 615;
const int BOTON1_2LOW = 627;
const int BOTON1_2HIGH = 699;
const int BOTON1_3LOW = 712;
const int BOTON1_3HIGH = 756;
const int BOTON1_4LOW = 787;
const int BOTON1_4HIGH = 838;
const int BOTON1_5LOW = 881;
const int BOTON1_5HIGH = 933;
const int BOTON1_6LOW = 950;
const int BOTON1_6HIGH = 1024;

// Grupo de botones del pin A2
const int BOTON2_1LOW = 520;
const int BOTON2_1HIGH = 596;
const int BOTON2_2LOW = 626;
const int BOTON2_2HIGH = 696;
const int BOTON2_3LOW = 708;
const int BOTON2_3HIGH = 780;
const int BOTON2_4LOW = 790;
const int BOTON2_4HIGH = 836;
const int BOTON2_5LOW = 890;
const int BOTON2_5HIGH = 930;
const int BOTON2_6LOW = 950;
const int BOTON2_6HIGH = 1024;

// Constantes velocidad del motor y tiempo (milisegundos)
const int IZQIZQARR_VEL = 255;
const int IZQIZQARR_TIE = 460;
const int IZQARR_VEL = 255;
const int IZQARR_TIE = 250;
const int DERARR_VEL = 255;
const int DERARR_TIE = 250;
const int DERDERARR_VEL = 255;
const int DERDERARR_TIE = 435;
const int IZQIZQABA_VEL = 255;
const int IZQIZQABA_TIE = 435;
const int IZQABA_VEL = 255;
const int IZQABA_TIE = 250;
const int DERABA_VEL = 255;
const int DERABA_TIE = 250;
const int DERDERABA_VEL = 255;
const int DERDERABA_TIE = 435;

// Variables para obtener lectura actual en cada pin
int lecturaBotones1;
int lecturaBotones2;
int lectura; // Para guardar el valor leido, indiferentemente del grupo
int grupoBotones; // 1 o 2, guarda el grupo al que pertenece el boton presionado
char posicion;
char posicionTemp; 

int maxLectura = 0; // Guarda el maximo del grupo de valores de cada presion de boton
boolean bLeyendo = false; // Es true si se estan leyendo valores al presionar un boton


/*     M O V    H O R I Z O N T A L   Y   R O T A C I O N    */

// Toggle Switch
const int switchPin = 2;   // Toggle Switch
int switchState;           // State of the switch

// Potenciometro
int const potenciometroPin = 0;  //pino analógico onde o potenciômetro está conectado
int valPotenciometro;            //variável usada para armazenar o valor lido no potenciômetro

/*       Motor DC
  Arduino          L293D
  ------------------------
  Pin 6            Pin 1
  Pin 3            Pin 2
  Pin 4            Pin 7
*/
const int pinEntrada1 = 3; // Input 1 de L293D (Pin 3 Arduino)
const int pinEntrada2 = 4; // Input 2 de L293D (Pin 4 Arduino)
const int pinVelocidad = 6;

// Servo
Servo servo1;  // Servo control object
const int pinServo = 9;

// Otros
boolean reversa;
int speed;         // Para la funcion serialSpeed
char serialPos;    // Para la funcion serialPosicion
boolean botonPresionado = true;  // Bandera para evitar multiples estiradas del arquero


/*     M O V    V E R T I C A L   */

Servo servo2;  // servo control object for pin 10
Servo servo3;  // servo control object for pin 11


void setup() {
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);
  apagarLEDsBotones();  //reset all register pins
  Serial.begin(9600);
 
  // Si el input analogo 3 esta desconectado, le da valores aleatorios a random 
  randomSeed(analogRead(3)); 
  lecturaBotones1 = 0;
  lecturaBotones2 = 0;
  lectura = 0;
  grupoBotones = 0;
  
  pinMode(switchPin, INPUT);  // Set up the switchPin to be an input  
  pinMode(pinEntrada1, OUTPUT);  
  pinMode(pinEntrada2, OUTPUT);  
  pinMode(pinVelocidad, OUTPUT); 
  servo1.attach(pinServo);
  servo1.write(90);    // Tell servo to go to 90 degrees
  
  servo2.attach(10); // Mano izq. del arquero (desde atras de el)
  servo3.attach(11);  

  inicializarArquero('H'); // Inicializar servos
}

void loop() {
  
  while (digitalRead(switchPin) == HIGH) {
    potCarretica();             // Potenciometro
    botonPresionado = true;
  }
  
  while (digitalRead(switchPin) == LOW) {
    //serialSpeed();              // Para calibrar
    //serialPosicion();             // Esta es la de a de veras
    leerControl();        
  }
}

/*F U N C I O N E S  M O V  H O R I Z O N T A L  Y  R O T A C I O N*/

void potCarretica() {
  valPotenciometro = analogRead(potenciometroPin);
  valPotenciometro = map(valPotenciometro, 0, 1023, -255, 255);
  valPotenciometro = constrain(valPotenciometro, -255, 255);
  
  if (valPotenciometro < 0){
    haciaAdelante();   
    valPotenciometro *= -1;
    reversa = true;
  } else if (valPotenciometro > 0){
    haciaAtras();
    reversa = false;
  }
  
  if (valPotenciometro < 50){
    
    analogWrite(pinVelocidad, 0); // Parar el motor
    Serial.println("Motor parado...");
    
  } else {
    
    valPotenciometro = map(valPotenciometro, 50, 255, 170, 230);
    valPotenciometro = constrain(valPotenciometro, 170, 230);
  
    analogWrite(pinVelocidad, valPotenciometro);
    if (reversa){
      Serial.print("Velocidad (R): ");
    } else {
      Serial.print("Velocidad:     ");
    }
    Serial.println(valPotenciometro);
  }
  delay(100);
  analogWrite(pinVelocidad, 0); // Parar el motor
  delay(2000);

}


void serialSpeed() {
  /* Funcion para graduar la velocidad y el delay que se necesita para estirar el arquero */
  
  // First we check to see if incoming data is available:
  while (Serial.available() > 0) {

    // If it is, we'll use parseInt() to pull out any numbers:
    speed = Serial.parseInt();
    
    // Manejar numeros negativos para echar el motor hacia atras
    if (speed < 0) {
      haciaAtras();
      speed *= -1;
      reversa = true;
    } else {
      haciaAdelante();
      reversa = false;
    }

    // Because analogWrite() only works with numbers from
    // 0 to 255, we'll be sure the input is in that range:
    speed = constrain(speed, 0, 255);
    
    // We'll print ohut a message to let you know that the
    // number was received:
    Serial.print("Nueva velocidad: ");
    if (reversa) {
        Serial.print(speed);
        Serial.println(" (en reversa)"); 
    } else { 
      Serial.println(speed);
    }

    // And finally, we'll set the speed of the motor!
    analogWrite(pinVelocidad, speed);
    //digitalWrite(pinVelocidad, HIGH);
    delay(400);                // delay for onTime milliseconds
    analogWrite(pinVelocidad, 0);  // Parar el motor antes de cambiar modo
  }
}

void haciaAdelante() {
  // Define la direccin del  Motor DC hacia adelante a traves del L293D
  digitalWrite(pinEntrada1,LOW);
  digitalWrite(pinEntrada2,HIGH);
}

void haciaAtras() {
  // Define la direccin del  Motor DC hacia atras a traves del L293D
  digitalWrite(pinEntrada1,HIGH);
  digitalWrite(pinEntrada2,LOW);
}


void serialPosicion() {
  /* Funcion para recibir posicion y estira el arquero hacia alla */
  
  // First we check to see if incoming data is available:
  while (Serial.available() > 0) {

    serialPos = Serial.read();
    
    Serial.print("Nueva posicion: ");
    Serial.println(serialPos);
    
    if (botonPresionado){
      botonPresionado = false;
      estirarArquero(serialPos);
      delay(5000);
      if (serialPos == 'A' || serialPos == 'B' || serialPos == 'C' || serialPos == 'D' || serialPos == 'E'){
        inicializarArquero('C');
      } else {
        inicializarArquero('H');
      }
    }
  }
}


/*  F U N C I O N E S   C O N T R O  L  */

void leerControl(){
  lecturaBotones1 = analogRead(pinBotones1);
  Serial.print("Grupo 1: ");
  Serial.println(lecturaBotones1);

  if (lecturaBotones1 > LIMITE_LECTURA_NULA) {
    grupoBotones = 1;
    lectura = lecturaBotones1;
  } else {
    lecturaBotones2 = analogRead(pinBotones2);
    Serial.print("Grupo 2: ");
    Serial.println(lecturaBotones2); 
    
    if (lecturaBotones2 > LIMITE_LECTURA_NULA) {
      grupoBotones = 2;
      lectura = lecturaBotones2;
    } else {
      lectura = 0; 
    }
  }

  // Manejar cuando un boton es presionado
  //   En ese caso se reciben aprox. 5 valores, puede haber valores que caen en el rango
  //   de otro boton. Sin embargo, el maximo valor si cae en el rango del boton presionado.
  if (lectura > LIMITE_LECTURA_NULA) {
    // Se ha presionado un boton
    bLeyendo = true;
    if (lectura > maxLectura) {
      maxLectura = lectura;
    }
  } else { // La lectura es nula
    if (bLeyendo == true) {
      // Se termina de leer, ya se puede prender LED y guardar region para estirar
      bLeyendo = false;  
      
      posicionTemp = obtenerPosicion(maxLectura, grupoBotones);
      if (posicionTemp == 'N') {
        Serial.print("Se obtuvo una posicion algo loca con la lectura... ");        
        Serial.print(maxLectura);        
        Serial.print(" del grupo de botones... ");        
        Serial.println(grupoBotones);    
      } else if (posicionTemp == 'Y') {
        // Estirar arquero a la posicion almacenada con anterioridad
        estirarArquero(posicion);  
        apagarLEDsBotones();
        Serial.print("Se estira el arquero a la posicion...");
        Serial.println(posicion);
        delay(4000);
        inicializarArquero(posicion);
      } else if (posicionTemp == 'R') { 
        // Elegir posicion aleatoriamente y almacenarla hasta que se presione el boton 'Y'
        posicion = posiciones[random(10)];
        while (posicion == 'H'){ // Descartar el centro como opcion para el mov. aleatorio
          posicion = posiciones[random(10)];
        }
        prenderLEDBoton('R');
        Serial.println("Elegir region aleatoriamente...");
      } else {     
        // Almacenar nueva posicion y prender su LED correspondiente
        posicion = posicionTemp;
        Serial.print("Se ha presionado el boton de la posicion...");
        Serial.println(posicion);
        prenderLEDBoton(posicion);
      }      

      // La lectura fue terminada, iniciar con otra nueva
      maxLectura = 0; 
    } // else: Viene de lectua nula y sigue en lectura nula (Nada pasa)
  }
  delay(50);  
}


char obtenerPosicion(int lectura, int grupoBotones) {
  // Devolver la posicion del boton presionado
  if (grupoBotones == 1)
    return obtenerPosicion1(lectura);
  else  if (grupoBotones == 2)
    return obtenerPosicion2(lectura);
  else
    Serial.println("WARNING: Se pide posicion de boton pero el grupo de botones es 0");
}

char obtenerPosicion1(int lectura) {
  if (lectura > BOTON1_6LOW && lectura < BOTON1_6HIGH) 
    return 'Y';
  else if (lectura > BOTON1_3LOW && lectura < BOTON1_3HIGH)
    return 'R';
  else if (lectura > BOTON1_4LOW && lectura < BOTON1_4HIGH)
    return 'J';
  else if (lectura > BOTON1_2LOW && lectura < BOTON1_2HIGH)
    return 'I';
  else if (lectura > BOTON1_5LOW && lectura < BOTON1_5HIGH)
    return 'E';
  else if (lectura > BOTON1_1LOW && lectura < BOTON1_1HIGH)
    return 'D';
  else 
    return 'N';
}

char obtenerPosicion2(int lectura) {
  if (lectura > BOTON2_6LOW && lectura < BOTON2_6HIGH) 
    return 'H';
  else if (lectura > BOTON2_2LOW && lectura < BOTON2_2HIGH)
    return 'F';
  else if (lectura > BOTON2_1LOW && lectura < BOTON2_1HIGH)
    return 'A';
  else if (lectura > BOTON2_4LOW && lectura < BOTON2_4HIGH)
    return 'G';
  else if (lectura > BOTON2_5LOW && lectura < BOTON2_5HIGH)
    return 'C';
  else if (lectura > BOTON2_3LOW && lectura < BOTON2_3HIGH)
    return 'B';
  else 
    return 'N';
}

void prenderLEDBoton(char pos) {
  // Prender LEDs de los botones
  int LED;
  for(int i = 0; i <  11; i++) {
    if (posiciones[i] == pos) {
      LED = LEDs[i];
      break;
    }
  }
  clearRegisters();
  setRegisterPin(LED, HIGH);
  writeRegisters(); 
  Serial.print("Se ha encendido el LED...");  
  Serial.println(LED);  
}

void apagarLEDsBotones() {
  clearRegisters();
  writeRegisters();
}



//set all register pins to LOW
void clearRegisters() {
  for(int i = numOfRegisterPins - 1; i >=  0; i--) {
     registers[i] = LOW;
  }
} 


//Set and display registers
//Only call AFTER all values are set how you would like (slow otherwise)
void writeRegisters() {

  digitalWrite(RCLK_Pin, LOW);

  for(int i = numOfRegisterPins - 1; i >=  0; i--) {
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);

  }
  digitalWrite(RCLK_Pin, HIGH);

}

//set an individual pin HIGH or LOW
void setRegisterPin(int index, int value) {
  registers[index] = value;
}







void estirarArquero(char pos) {
  moverAPosicion(pos);  
}

void moverAPosicion(char pos) {
  /*     _______________
        | A  B  C  D  E |
        | F  G  H  I  J |
    =========================
  */
  
  switch (pos) {
    case 'A':  // Izq-Izq-Arr
      izqIzqArr();
      break;
    case 'B':  // Izq-Arr
      izqArr();
      break;
    case 'C':  // Arr
      arr();
      break;
    case 'D':  // Der-Arr
      derArr();
      break;
    case 'E':  // Der-Der-Arr
      derDerArr();
      break;
    case 'F':  // Izq-Izq-Aba    
      izqIzqAba();      
      break;
    case 'G':  // Izq-Aba
      izqAba();
      break;
    case 'H':  // Quieto
      quieto();
      break;
    case 'I':  // Der-Aba
      derAba();
      break;
    case 'J':  // Der-Der-Aba
      derDerAba();
      break;    
  } 
}


void izqIzqArr() {       // A
  haciaAtras();    
  analogWrite(pinVelocidad, IZQIZQARR_VEL);
  servo1.write(35);    
  subir();
  delay(IZQIZQARR_TIE);                    // delay for some milliseconds  
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}

void izqArr() {          // B
  subir();
  haciaAtras();    
  analogWrite(pinVelocidad, IZQARR_VEL);
  servo1.write(80); 
  delay(IZQARR_TIE);                    // delay for some milliseconds
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}

void arr() {             // C
  // Poleas arriba
  subir();
}

void derArr() {          // D
  subir();
  haciaAdelante();    
  analogWrite(pinVelocidad, DERARR_VEL);
  servo1.write(100);
  delay(DERARR_TIE);                    // delay for some milliseconds
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}

void derDerArr() {       // E
  haciaAdelante();    
  analogWrite(pinVelocidad, DERDERARR_VEL);
  servo1.write(135);
  subir();  
  delay(DERDERARR_TIE);                    // delay for some milliseconds 
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}

void izqIzqAba() {       // F
  haciaAtras();    
  analogWrite(pinVelocidad, IZQIZQABA_VEL);
  servo1.write(10);    
  delay(IZQIZQABA_TIE);                    // delay for some milliseconds  
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}

void izqAba() {          // G
  haciaAtras();    
  analogWrite(pinVelocidad, IZQABA_VEL);
  delay(IZQABA_TIE);                    // delay for some milliseconds
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}

void quieto() {          // H
  // No haga nada
}

void derAba() {          // I
  haciaAdelante();    
  analogWrite(pinVelocidad, DERABA_VEL);
  delay(DERABA_TIE);                    // delay for some milliseconds
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}

void derDerAba() {       // J
  haciaAdelante();    
  analogWrite(pinVelocidad, DERDERABA_VEL);
  servo1.write(170);    
  delay(DERDERABA_TIE);                    // delay for some milliseconds 
  analogWrite(pinVelocidad, 0);  // Parar el motor 
}



void inicializarArquero(char pos) {
  String zona = "";
  switch (pos) {
    case 'A':  // Izq-Izq-Arr
      zona = "izqizqarr";
      break;
    case 'B':  // Izq-Arr
      zona = "izqarr";
      break;
    case 'C':  // Arr
      zona = "arr";
      break;
    case 'D':  // Der-Arr
      zona = "derarr";
      break;
    case 'E':  // Der-Der-Arr
      zona = "derderarr";
      break;
    case 'F':  // Izq-Izq-Aba    
      zona = "izqizqaba";    
      break;
    case 'G':  // Izq-Aba
      zona = "izqaba";
      break;
    case 'H':  // Quieto
      zona = "qui";
      break;
    case 'I':  // Der-Aba
      zona = "deraba";
      break;
    case 'J':  // Der-Der-Aba
      zona = "derderaba";
      break;    
  } 
  
  if (zona.endsWith("arr")) {
    bajar();    
    
    if (zona.startsWith("derder")) {
      servo1.write(90);
      haciaAtras();    
      analogWrite(pinVelocidad, DERDERARR_VEL);
      delay(DERDERARR_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor 
    } else if (zona.startsWith("der")) {
      haciaAtras();    
      analogWrite(pinVelocidad, DERARR_VEL);
      delay(DERARR_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor   
    } else if (zona.startsWith("izqizq")) {
      haciaAdelante();    
      analogWrite(pinVelocidad, IZQIZQARR_VEL);
      delay(IZQIZQARR_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor 
    } else if (zona.startsWith("izq")) {
      haciaAdelante();    
      analogWrite(pinVelocidad, IZQARR_VEL);
      delay(IZQARR_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor   
    } 

    servo1.write(90);
    
  } else {
    inicializarServosMovVertical();   
    
    if (zona.startsWith("derder")) {
      haciaAtras();    
      analogWrite(pinVelocidad, DERDERABA_VEL);
      delay(DERDERABA_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor 
    } else if (zona.startsWith("der")) {
      haciaAtras();    
      analogWrite(pinVelocidad, DERABA_VEL);
      delay(DERABA_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor   
    } else if (zona.startsWith("izqizq")) {
      haciaAdelante();    
      analogWrite(pinVelocidad, IZQIZQABA_VEL);
      delay(IZQIZQABA_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor 
    } else if (zona.startsWith("izq")) {
      haciaAdelante();    
      analogWrite(pinVelocidad, IZQABA_VEL);
      delay(IZQABA_TIE);                    // delay for some milliseconds  
      analogWrite(pinVelocidad, 0);  // Parar el motor   
    } 
    
    servo1.write(90);
  }
}

void inicializarServosMovVertical() {
  servo2.write(0);   
  servo3.write(180); 
}

void subir() {
  servo2.write(180);   
  servo3.write(0);
}

void bajar() {
  for(int grados = 180; grados >= 0; grados -= 2) {                                
    servo2.write(grados);  
    servo3.write(180-grados);  
    delay(20);               // Short pause to allow it to move
  }
}
