/*
  German Carrillo, December 2015
  geotux_tuxman [at] linuxmail.org
  Shared under license GPL v.2.0
*/
const int pinVelocidad = 3;

void setup() {
  pinMode(pinVelocidad, OUTPUT);
  analogWrite(pinVelocidad, 255); // Let's start with full torque
  delay(200);  
  analogWrite(pinVelocidad, 80); // We want the motor to rotate slowly
}

void loop() {
  // We don't need anything here
}
