// Include the required Wire library for I2C<br>#include 

#include <Wire.h> 

int x = 0;
void setup() {
  // Start the I2C Bus as Master
  Wire.begin(); 
}
void loop() {
  // sample button pressed
  // store in x
  // send x
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(x);              // sends x 
  Wire.endTransmission();    // stop transmitting
  delay(500);
}
