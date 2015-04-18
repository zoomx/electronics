/*
German Carrillo, Agosto de 2013
geotux_tuxman [at] linuxmail.org
*/

// Arreglo con pines para cada LED, 3 pines por LED: RGB
const int numLEDs = 3;
const int numPines = numLEDs*3;
const int pinesLEDs[numPines] = {7,6,5,10,9,8,2,0,1};// Arduino: {9,10,11,8,7,6,5,4,3}
//{3,4,5,0,1,2,8,10,9}

// Definicion de colores:
const int rojo[3] = {HIGH,LOW,LOW};
const int verde[3] = {LOW,HIGH,LOW};
const int azul[3] = {LOW,LOW,HIGH};
const int blanco[3] = {HIGH,HIGH,HIGH};
const int amarillo[3] = {HIGH,HIGH,LOW};
const int violeta[3] = {HIGH,LOW,HIGH};
const int cian[3] = {LOW,HIGH,HIGH};
const int apagado[3] = {LOW,LOW,LOW};

// Otros
int todos[numLEDs] = {1,2,3};


void setup()
{
  for(int i=0; i<numPines; i++)
    pinMode(pinesLEDs[i], OUTPUT);
}


void loop()
{
  for(int i=0; i<7; i++) // 15s. aprox.
    bandera();
  for(int i=0; i<20; i++)  // 30s. aprox.  
    navidad();
  for(int i=0; i<2;i++) // 15s. aprox.
    mainColors();
}


void bandera() {
  definirColor(todos, apagado);
  delay(1000);

  // LEDs: 1 amarillo, 2 azul, 3 rojo
  definirColor(1, amarillo);
  definirColor(2, azul);
  definirColor(3, rojo); 

  delay(1000); 
}

void navidad(){
  //rojo1,verde2,blanco3,
  definirColor(1, rojo);
  definirColor(2, verde);
  definirColor(3, blanco); 
  delay(500);  
  definirColor(1, blanco);
  definirColor(2, rojo);
  definirColor(3, verde); 
  delay(500);  
  definirColor(1, verde);
  definirColor(2, blanco);
  definirColor(3, rojo); 
  delay(500);
}

// This function displays the eight "main" colors that the RGB LED
// can produce. If you'd like to use one of these colors in your 
// own sketch, you cancopy and paste that section into your code.
void mainColors() {
  // Off (all LEDs off):
  // definirColor(todos,apagado);  
  // delay(1000);

  // Red (turn just the red LED on):
  definirColor(todos,rojo);

  delay(1000);

  // Green (turn just the green LED on):
  definirColor(todos,verde);
  
  delay(1000);

  // Blue (turn just the blue LED on):
  definirColor(todos,azul);
  
  delay(1000);

  // Yellow (turn red and green on):
  definirColor(todos,amarillo);
  delay(1000);

  // Cyan (turn green and blue on):
  definirColor(todos,cian);  
  
  delay(1000);

  // Purple (turn red and blue on):
  definirColor(todos,violeta);
  
  delay(1000);

  // White (turn all the LEDs on):
  definirColor(todos,blanco);

  delay(1000);
}

void definirColor(int led, const int color[]){
  digitalWrite(pinesLEDs[(led-1)*numLEDs], color[0]);
  digitalWrite(pinesLEDs[(led-1)*numLEDs+1], color[1]);
  digitalWrite(pinesLEDs[(led-1)*numLEDs+2], color[2]);
}

void definirColor(int led[], const int color[]){
  for(int i=0; i<numLEDs; i++){ // numLEDs is the size of the array led, replace with sth more generic
    definirColor(led[i],color);
  }
}
