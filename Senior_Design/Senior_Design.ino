/*  Code for BME 371 Senior Design
    Team 18:
    Shivani Gupta, Redon Kallashi, Hannah Wang, Enoch Wong, Le Hoang
    Adapted from and Modified:
    - Simple Stepper Motor Control Example Code, by Dejan Nedelkovski
    - Utilized NewPing Library
*/

// Library Includes
#include <Wire.h>
#include <NewPing.h>
#include <Adafruit_PWMServoDriver.h>

// ************ Initialization ************
// INSERTION: Ultrasonic Sensor
// Using the NewPing Library
#define TRIGGER_PIN   12 // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN      11 // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters)

// TRANSITIONS: Servo Motors

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  500 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 0;

int x = 0;

// WAITING RACK: defines pins numbers
const int stepPin = 3;
const int dirPin = 4;

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


// ************ Set Up ************
void setup() {
  // Set up the serial monitor:
  //  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.

  // Start the I2C Bus as Slave on address 9
  Wire.begin(9);
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);

  // INSERTION: Ultrasonic Sensor
  pingTimer = millis(); // Start now.

  // TRANSITIONS: Servo Motors
  //  Serial.println("Servo test!");
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  delay(10);
  //  servoAdvance(1);
  //  servoAdvance(2);

  // WAITING RACK: Stepper Motor
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
}

// ************ Loop ************
void loop() {
  // INSERTION: Ultrasonic Sensor
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }

  //If value received is 0 blink LED for 200 ms
  if (x == '1') { // Lav
    // Y-TUBE: Start
    //    Serial.println("Y-Tube:");
    servoRetract(0);
    delay(1000);
    servoAdvance(0);
    delay(1000);
    pwm.setPWM(0, 0, 0);
  }
  //If value received is 3 blink LED for 400 ms
  if (x == '2') { // Gold
    // TRANSITIONS: Servo Motors
    if ((sonar.ping_result / US_ROUNDTRIP_CM >= 6) && (sonar.ping_result / US_ROUNDTRIP_CM < 10)) { // start sliding door

      // Y-TUBE: Start
      //    Serial.println("Y-Tube:");
      servoRetract(0);
      delay(1000);
      servoAdvance(0);
      delay(1000);
      pwm.setPWM(0, 0, 0);

      // HOLDER: Start
      //    Serial.println("Holder:");
      servoRetract(1);
      delay(1000);
      servoAdvance(1);
      delay(1000);
      servoRetract(1);
      delay(1000);
      pwm.setPWM(1, 0, 0);
      // WAITING RACK: Stepper Motor
      // Start the timer:
      // Move Waiting Rack:
      //    Serial.println("Spinning:");
      WR_test_onerotation();
      delay(100);

      // TO CENTRIFUGE: Start
      //    Serial.println("To Centrifuge:");
      servoAdvance(2);
      delay(1000);
      servoRetract(2);
      delay(1000);
      pwm.setPWM(2, 0, 0);

      for (int x = 0; x < 6; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(5000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(5000);
      }

      //  AFTER CENTRIFUGE: Start
      //  Serial.println("END:");
      servoRetract(4);
      delay(1000);
      servoAdvance(4);
      delay(1000);
      servoRetract(4);
      delay(1000);
      pwm.setPWM(4, 0, 0);

      sonar.ping_result = 1;
    }
  }
  //  delay(30000);
} // Loop ends here


// ************ Private Helper Functions ************

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    //    Serial.print("Ping: ");
    //    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    //    Serial.println("cm");
  }
  // Don't do anything here!
}

// delay to receive tube
// move to next step: add ten second delay timer
// move to the final position
void WR_test_onerotation() {
  digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for (int x = 0; x < 92.5; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(5000);
  }
  delay(3000); // One second delay

}

void servoRetract(uint8_t servoNum) {
  int max = SERVOMAX;
  if (servoNum == 2) {
    max = 600;
  }
  for (uint16_t pulselen = SERVOMIN; pulselen < max; pulselen++) {
    pwm.setPWM(servoNum, 0, pulselen);
  }
  delay(500);
}

void servoAdvance(uint8_t servoNum) {
  int max = SERVOMAX;
  if (servoNum == 2) {
    max = 600;
  }
  for (uint16_t pulselen = max; pulselen > SERVOMIN; pulselen--) {
    pwm.setPWM(servoNum, 0, pulselen);
  }
  delay(500);
}

// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit");
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}
