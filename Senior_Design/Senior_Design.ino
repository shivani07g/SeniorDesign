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
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 0;

// WAITING RACK: defines pins numbers
const int stepPin = 3;
const int dirPin = 4;

unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


// ************ Set Up ************
void setup() {
  // Set up the serial monitor:
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.

  // INSERTION: Ultrasonic Sensor
  pingTimer = millis(); // Start now.

  // TRANSITIONS: Servo Motors
  Serial.println("Servo test!");
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  delay(10);

  // WAITING RACK: Stepper Motor
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

// ************ Loop ************
void loop() {
  // INSERTION: Ultrasonic Sensor
  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }

  // TRANSITIONS: Servo Motors
  if ((sonar.ping_result / US_ROUNDTRIP_CM >= 4) && (sonar.ping_result / US_ROUNDTRIP_CM < 10)) { // start sliding door
    Serial.println("Transition:");
    servoRetract(0);
    delay(2000);
    servoAdvance(0);
  }

  // WAITING RACK: Stepper Motor
  if ((sonar.ping_result / US_ROUNDTRIP_CM >= 4) && (sonar.ping_result / US_ROUNDTRIP_CM < 10)) { // start one rotation of waiting rack
    // Start the timer:
    // Move Waiting Rack:
    Serial.println("Spinning:");
    WR_test_onerotation();
  }
}

// ************ Private Helper Functions ************

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    Serial.print("Ping: ");
    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
  }
  // Don't do anything here!
}

void WR_test_onerotation() {
  digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for (int x = 0; x < 2000; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
  delay(1000); // One second delay
}

void servoRetract(uint8_t servoNum){
    for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
      pwm.setPWM(servoNum, 0, pulselen);
    }
    delay(500);  
}

void servoAdvance(uint8_t servoNum){
    for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
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
