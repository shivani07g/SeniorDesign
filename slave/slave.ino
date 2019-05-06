// Include the required Wire library for I2C<br>

#include <Wire.h>


int LED = 13;
int x = 0;
void setup() {
  // Define the LED pin as Output
  Serial.begin(9600);
  pinMode (LED, OUTPUT);
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9);
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
}
void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
}
void loop() {
    Serial.println(x);
  //If value received is 0 blink LED for 200 ms
  if (x == '1') {

  }
  //If value received is 3 blink LED for 400 ms
  if (x == '2') {

  }
}
