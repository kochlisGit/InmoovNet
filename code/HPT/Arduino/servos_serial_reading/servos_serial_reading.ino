#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define PORT 115200
#define TIMEOUT 15000
#define SERVOMIN_S92R 90         // Updated constant
#define SERVOMAX_S92R 535        // Updated constant

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

void setup() {
    Serial.begin(PORT);
    Serial.setTimeout(TIMEOUT);

    Serial.println("---- Setting up PWM ---");
    while (!pwm.begin()) {
        Serial.println("Failed to find PWM chip, retrying...");
        delay(1000);
    }
    pwm.setPWMFreq(50);
}

void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();  // Remove extra spaces or newlines

        // Check if the reset command is received
        if (input == "reset fingers") {
            Serial.println("Reset command received. Resetting motor angles.");

            float motor_angles[5] = {160.0, 0.0, 160.0, 160.0, 0.0}; // Reset positions

            // Print motor angles before applying them
            Serial.print("Applying Reset Motor Angles: ");
            for (int i = 0; i < 5; i++) {
                Serial.print(motor_angles[i]);
                if (i < 4) Serial.print(", ");
            }
            Serial.println();

            // Apply motor angles to servos
            int pulse1 = map(motor_angles[0], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
            int pulse2 = map(motor_angles[1], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
            int pulse3 = map(motor_angles[2], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
            int pulse4 = map(motor_angles[3], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
            int pulse5 = map(160.0- motor_angles[4], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);

            pwm.setPWM(0, 0, pulse1);
            pwm.setPWM(1, 0, pulse2);
            pwm.setPWM(2, 0, pulse3);
            pwm.setPWM(3, 0, pulse4);
            pwm.setPWM(4, 0, pulse5);

            Serial.println("Servos reset.");
            return;
        }

        float motor_angles[5];
        int motorIndex = 0;

        // Parse received data (comma-separated motor angles)
        char* token = strtok((char*)input.c_str(), ",");
        while (token != NULL && motorIndex < 5) {
            motor_angles[motorIndex++] = atof(token);
            token = strtok(NULL, ",");
        }

        // Validate motor angles
        bool valid = true;
        for (int i = 0; i < 4; i++) {  // Motors 0-3 should be in [0, 160]
            if (motor_angles[i] < 0 || motor_angles[i] > 160) {
                valid = false;
                break;
            }
        }
        if (motor_angles[4] < 0 || motor_angles[4] > 100) {  // Motor 4 should be in [0, 100]
            valid = false;
        }

        // If invalid, skip this iteration
        if (!valid) {
            Serial.println("Invalid motor angles received. Skipping...");
            return;
        }

        // Print motor angles before applying them
        Serial.print("Applying Motor Angles: ");
        for (int i = 0; i < 5; i++) {
            Serial.print(motor_angles[i]);
            if (i < 4) Serial.print(", ");
        }
        Serial.println();

        // Apply motor angles to servos
        int pulse1 = map(motor_angles[0], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
        int pulse2 = map(motor_angles[1], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
        int pulse3 = map(motor_angles[2], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
        int pulse4 = map(motor_angles[3], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
        int pulse5 = map(160.0- motor_angles[4], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);

        pwm.setPWM(0, 0, pulse1);
        pwm.setPWM(1, 0, pulse2);
        pwm.setPWM(2, 0, pulse3);
        pwm.setPWM(3, 0, pulse4);
        pwm.setPWM(4, 0, pulse5);

        Serial.println("Servos updated.");
        
    }
}


