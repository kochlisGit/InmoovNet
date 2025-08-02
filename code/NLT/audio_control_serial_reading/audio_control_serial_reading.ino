#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define PORT 115200
#define TIMEOUT 15000
#define SERVOMIN_S92R 90         // Updated constant
#define SERVOMAX_S92R 535        // Updated constant
#define INTER_MOVE_DELAY 500     // Delay between angle sets

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Initial positions for reset
const float INITIAL_POSITIONS[5] = {160.0, 0.0, 160.0, 160.0, 0.0};

void setup() {
    Serial.begin(PORT);
    Serial.setTimeout(TIMEOUT);

    Serial.println("---- Setting up PWM ---");
    while (!pwm.begin()) {
        Serial.println("Failed to find PWM chip, retrying...");
        delay(1000);
    }
    pwm.setPWMFreq(50);
    Serial.println("Arduino ready");
}

void applyAngles(float angles[5]) {
    // Print motor angles before applying them
    Serial.print("Applying Motor Angles: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(angles[i]);
        if (i < 4) Serial.print(", ");
    }
    Serial.println();

    // Apply motor angles to servos
    int pulse1 = map(angles[0], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
    int pulse2 = map(angles[1], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
    int pulse3 = map(angles[2], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
    int pulse4 = map(angles[3], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
    int pulse5 = map(angles[4], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);

    // pwm.setPWM(0, 0, pulse1);
    // pwm.setPWM(1, 0, pulse2);
    // pwm.setPWM(2, 0, pulse3);
    // pwm.setPWM(3, 0, pulse4);
    // pwm.setPWM(4, 0, pulse5);
}

void resetToInitialPositions() {
    Serial.println("Reset command received. Resetting motor angles.");
    float motor_angles[5];
    memcpy(motor_angles, INITIAL_POSITIONS, sizeof(motor_angles));
    applyAngles(motor_angles);
    Serial.println("Servos reset");
}

void processAngleSets(String input) {
    // Split into individual angle sets (separated by semicolons)
    int setStart = 0;
    int setEnd = input.indexOf(';');
    int setsProcessed = 0;
    
    while (setEnd != -1 || (setStart < input.length() && setsProcessed == 0)) {
        String angleSet;
        if (setEnd == -1) {
            angleSet = input.substring(setStart);
        } else {
            angleSet = input.substring(setStart, setEnd);
        }
        
        // Parse individual angles in this set
        float motor_angles[5] = {0};
        int motorIndex = 0;
        
        int angleStart = 0;
        int angleEnd = angleSet.indexOf(',');
        
        while (angleEnd != -1 && motorIndex < 5) {
            motor_angles[motorIndex++] = angleSet.substring(angleStart, angleEnd).toFloat();
            angleStart = angleEnd + 1;
            angleEnd = angleSet.indexOf(',', angleStart);
        }
        
        // Get last angle if there's remaining data
        if (motorIndex < 5 && angleStart < angleSet.length()) {
            motor_angles[motorIndex] = angleSet.substring(angleStart).toFloat();
        }
        
        // Validate motor angles
        bool valid = true;
        for (int i = 0; i < 4; i++) {  // Motors 0-3
            if (motor_angles[i] < 0 || motor_angles[i] > 160) {
                valid = false;
                break;
            }
        }
        if (motor_angles[4] < 0 || motor_angles[4] > 100) {  // Motor 4
            valid = false;
        }
        
        if (!valid) {
            Serial.println("Invalid motor angles in set. Skipping...");
            return;
        }
        
        // Apply this angle set
        applyAngles(motor_angles);
        setsProcessed++;
        
        // Delay between sets (except after last one)
        if (setEnd != -1) {
            delay(INTER_MOVE_DELAY);
        }
        
        // Move to next set
        setStart = setEnd + 1;
        setEnd = input.indexOf(';', setStart);
    }
    
    Serial.println("Servos updated");
}

void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        
        if (input == "reset fingers") {
            resetToInitialPositions();
            return;
        }
        
        processAngleSets(input);
    }
}

