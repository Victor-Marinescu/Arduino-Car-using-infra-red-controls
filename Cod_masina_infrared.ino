// Pin Definitions
const int X_AXIS = A3;
const int Y_AXIS = A5;
const int LIGHT_SENSOR = A0;
const int LED = A4;

const int RIGHT_MOTOR_CTRL = 2;
const int RIGHT_MOTOR_PWR = 5;
const int LEFT_MOTOR_CTRL = 4;
const int LEFT_MOTOR_PWR = 6;

const int FRONT_TRIGGER_PIN = 10;
const int FRONT_ECHO_PIN = 9;
const int REAR_TRIGGER_PIN = 12;
const int REAR_ECHO_PIN = 13;

// Thresholds and constants
const int DEADZONE = 50;
const int LIGHT_THRESHOLD = 500;
const int MAX_SPEED = 255;
const long OBSTACLE_THRESHOLD_CM = 15;

// Variables
int joystickX = 0;
int joystickY = 0;

// MotorMovement class
class MotorMovement {
public:
  void moveForward(int speed) {
    digitalWrite(RIGHT_MOTOR_CTRL, HIGH);
    digitalWrite(LEFT_MOTOR_CTRL, HIGH);
    analogWrite(RIGHT_MOTOR_PWR, speed);
    analogWrite(LEFT_MOTOR_PWR, speed);
  }

  void moveBackward(int speed) {
    digitalWrite(RIGHT_MOTOR_CTRL, LOW);
    digitalWrite(LEFT_MOTOR_CTRL, LOW);
    analogWrite(RIGHT_MOTOR_PWR, speed);
    analogWrite(LEFT_MOTOR_PWR, speed);
  }

  void turnRight(int speed) {
    digitalWrite(RIGHT_MOTOR_CTRL, LOW);
    digitalWrite(LEFT_MOTOR_CTRL, HIGH);
    analogWrite(RIGHT_MOTOR_PWR, speed);
    analogWrite(LEFT_MOTOR_PWR, speed);
  }

  void turnLeft(int speed) {
    digitalWrite(RIGHT_MOTOR_CTRL, HIGH);
    digitalWrite(LEFT_MOTOR_CTRL, LOW);
    analogWrite(RIGHT_MOTOR_PWR, speed);
    analogWrite(LEFT_MOTOR_PWR, speed);
  }

  void stopMotors() {
    analogWrite(RIGHT_MOTOR_PWR, 0);
    analogWrite(LEFT_MOTOR_PWR, 0);
    digitalWrite(RIGHT_MOTOR_CTRL, LOW);
    digitalWrite(LEFT_MOTOR_CTRL, LOW);
  }
};

// UltrasonicSensor class
class UltrasonicSensor {
public:
  long getDistance(int triggerPin, int echoPin) {
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
  }
};

// Create objects
MotorMovement motor;
UltrasonicSensor ultrasonic;

void setup() {
  // Initialize pins
  pinMode(LED, OUTPUT);
  pinMode(RIGHT_MOTOR_CTRL, OUTPUT);
  pinMode(RIGHT_MOTOR_PWR, OUTPUT);
  pinMode(LEFT_MOTOR_CTRL, OUTPUT);
  pinMode(LEFT_MOTOR_PWR, OUTPUT);
  pinMode(FRONT_TRIGGER_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  pinMode(REAR_TRIGGER_PIN, OUTPUT);
  pinMode(REAR_ECHO_PIN, INPUT);

  Serial.begin(9600); // For debugging
}

void loop() {
  joystickX = analogRead(X_AXIS) - 512;
  joystickY = analogRead(Y_AXIS) - 512;

  int lightLevel = analogRead(LIGHT_SENSOR);
  digitalWrite(LED, lightLevel < LIGHT_THRESHOLD ? HIGH : LOW);

  if (abs(joystickX) < DEADZONE && abs(joystickY) < DEADZONE) {
    motor.stopMotors(); // Full stop
    return;
  }

  long frontDistance = ultrasonic.getDistance(FRONT_TRIGGER_PIN, FRONT_ECHO_PIN);
  long rearDistance = ultrasonic.getDistance(REAR_TRIGGER_PIN, REAR_ECHO_PIN);

  // Miscare fata-spate
  if (joystickY > DEADZONE && frontDistance > OBSTACLE_THRESHOLD_CM) {
    motor.moveForward(map(joystickY, DEADZONE, 512, 0, MAX_SPEED));
  } else if (joystickY < -DEADZONE && rearDistance > OBSTACLE_THRESHOLD_CM) {
    motor.moveBackward(map(abs(joystickY), DEADZONE, 512, 0, MAX_SPEED));
  } 
  // Miscare rotativa
  else if (joystickX > DEADZONE) {
    motor.turnRight(map(joystickX, DEADZONE, 512, 0, MAX_SPEED));
  } else if (joystickX < -DEADZONE) {
    motor.turnLeft(map(abs(joystickX), DEADZONE, 512, 0, MAX_SPEED));
  } else {
    motor.stopMotors();
  }
}
