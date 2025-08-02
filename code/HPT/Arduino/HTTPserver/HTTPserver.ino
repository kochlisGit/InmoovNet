#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Define constants
#define PORT 115200
#define TIMEOUT 15000
#define SSID "Redmi Note 9 Pro"           // Replace with your Wi-Fi SSID
#define PASSWORD "12345678910"   // Replace with your Wi-Fi Password
#define SERVOMIN_S92R 90 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX_S92R 535 // this is the 'maximum' pulse length count (out of 4096)

// HTTP server instance on port 80
WebServer server(80);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver (0x40);

// Variables for the Server
float motorAngles[5] = {-10.0, -10.0, -10.0, -10.0, -10.0}; // Array for 5 motor angles
bool pythonStarted = false;                       // Indicates if the Python script has started
bool datasetComplete = false;                     // Indicates if all combinations are processed
bool combinationProcessed = false;                // Tracks whether the current combination is processed

// Define parameters for motor angle combinations
const int distinctValues = 5; // Number of distinct values for each motor angle
const float minValue = 0.0;   // Minimum value for motor angles
const float maxValue = 160.0; // Maximum value for motor angles
const float step = (maxValue - minValue) / (distinctValues - 1);
const int epsilon = 5;


// Handle GET request to "/motorAngles"
void handleMotorAngles() {
    String response = "";
    for (int i = 0; i < 5; i++) {
        response += String(motorAngles[i]);
        if (i < 4) response += ", "; // Add comma between values
    }
    server.send(200, "text/plain", response);
}

// Handle GET request to "/pythonStarted"
void handlePythonStarted() {
    server.send(200, "text/plain", pythonStarted ? "True" : "False");
}

// Handle GET request to "/datasetComplete"
void handleDatasetComplete() {
    server.send(200, "text/plain", datasetComplete ? "True" : "False");
}

// Handle GET request to "/combinationProcessed"
void handleCombinationProcessed() {
    server.send(200, "text/plain", combinationProcessed ? "True" : "False");
}

// Handle POST request to "/setMotorAngles"
void handleSetMotorAngles() {
    if (server.hasArg("motorAngles")) {
        String anglesArg = server.arg("motorAngles");
        int angleCount = 0;
        
        // Parse the angles from the input string
        int startIdx = 0;
        int commaIdx = anglesArg.indexOf(",");
        while (commaIdx != -1 && angleCount < 5) {
            motorAngles[angleCount] = anglesArg.substring(startIdx, commaIdx).toFloat();
            angleCount++;
            startIdx = commaIdx + 1;
            commaIdx = anglesArg.indexOf(",", startIdx);
        }
        // Add the last angle
        if (angleCount < 5) {
            motorAngles[angleCount] = anglesArg.substring(startIdx).toFloat();
        }

        server.send(200, "text/plain", "Motor angles updated successfully");
    } else {
        server.send(400, "text/plain", "Missing motorAngles parameter");
    }
}

// Handle POST request to "/setPythonStarted"
void handleSetPythonStarted() {
    if (server.hasArg("pythonStarted")) {
        String startedValue = server.arg("pythonStarted");
        pythonStarted = (startedValue == "True");
        server.send(200, "text/plain", "Python Started flag updated successfully");
    } else {
        server.send(400, "text/plain", "Missing pythonStarted parameter");
    }
}

// Handle POST request to "/setCombinationProcessed"
void handleSetCombinationProcessed() {
    Serial.println(server.hasArg("combinationProcessed"));
    if (server.hasArg("combinationProcessed")) {
        String processedValue = server.arg("combinationProcessed");
        combinationProcessed = (processedValue == "True");
        server.send(200, "text/plain", "Combination Processed flag updated successfully");
    } else {
        Serial.println("error 400");
        server.send(400, "text/plain", "Missing combinationProcessed parameter");
    }
}


// Setup function
void setup() {
    // Initialize serial communication
    Serial.begin(PORT);
    Serial.setTimeout(TIMEOUT);

    Serial.println("Connecting to WiFi...");

    // Wi-Fi connection
    int notConnected;
    do {
        WiFi.begin(SSID, PASSWORD); // Start Wi-Fi connection
        delay(5000);                // Wait for 5 seconds
        notConnected = WiFi.status() != WL_CONNECTED;

        if (notConnected) {
            Serial.println("Wrong WiFi credentials or unable to connect. Retrying...");
        }
    } while (notConnected);

    // Successfully connected to Wi-Fi
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // Print the IP address

    // Setting up HTTP server
    Serial.println("Starting HTTP server...");
    server.on("/motorAngles", handleMotorAngles); // Define GET endpoint
    server.on("/pythonStarted", handlePythonStarted); // Define GET endpoint
    server.on("/datasetComplete", handleDatasetComplete); // Define GET endpoint
    server.on("/combinationProcessed", handleCombinationProcessed); // Define GET endpoint

    server.on("/setMotorAngles", HTTP_POST, handleSetMotorAngles); // Define POST endpoint
    server.on("/setPythonStarted", HTTP_POST, handleSetPythonStarted); // Define POST endpoint
    server.on("/setCombinationProcessed", HTTP_POST, handleSetCombinationProcessed); // Define POST endpoint
    
    server.begin(); // Start the server
    Serial.println("HTTP server started!");

    pwm.begin();
    pwm.setPWMFreq(50);
    delay(10000);

}

void loop() {

    server.handleClient(); // Handle HTTP requests

    if (!pythonStarted) {
        // If the Python script hasn't started
        Serial.println("Python script has not started. Waiting...");
        delay(5000);
    }
    else if (pythonStarted && !datasetComplete) {

        Serial.println("Python script running. Dataset is being processed...");

        // Generate all combinations of motorAngles
        for (int a = 0; a < distinctValues; a++) {
            motorAngles[0] = minValue + a * step;
            for (int b = 0; b < distinctValues; b++) {
                motorAngles[1] = minValue + b * step;
                for (int c = 0; c < distinctValues; c++) {
                    motorAngles[2] = minValue + c * step;
                    for (int d = 0; d < distinctValues; d++) {
                        motorAngles[3] = minValue + d * step;
                        for (int e = 0; e < distinctValues; e++) {
                            motorAngles[4] = minValue + e * step;

                            // Print the current combination
                            Serial.print("Current combination: ");
                            for (int i = 0; i < 5; i++) {
                                Serial.print(motorAngles[i]);
                                if (i < 4) Serial.print(", ");
                            }
                            Serial.println();

                            Serial.println("Moving the hand...");

                            int pulse1 = map(motorAngles[0], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
                            int pulse2 = map(motorAngles[1], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
                            int pulse3 = map(motorAngles[2], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
                            int pulse4 = map(motorAngles[3], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);
                            int pulse5 = map(motorAngles[4], 0, 180, SERVOMIN_S92R, SERVOMAX_S92R);

                            pwm.setPWM(0,0,pulse1);
                            pwm.setPWM(1,0,pulse2);
                            pwm.setPWM(2,0,pulse3);
                            pwm.setPWM(3,0,pulse4);
                            pwm.setPWM(4,0,pulse5);

                            //delay(1000);

                            bool printCombinationMessage = false;
                            bool increaseAngles = false;
                            // Wait until combinationProcessed is set to True by Python
                            while (!combinationProcessed) {
                              server.handleClient(); // Handle incoming HTTP requests
                              if (!printCombinationMessage){
                                Serial.println("Waiting for Python to process the combination...");
                                printCombinationMessage = true;
                              }


                            }

                            // Reset combinationProcessed for the next combination
                            combinationProcessed = false;
                        }
                    }
                }
            }
        }

        // All combinations processed
        datasetComplete = true;
        Serial.println("All combinations processed. Dataset complete!");
    }

    if (datasetComplete) {
        Serial.println("Dataset has already been created. Nothing more to do.");
        delay(5000); // Delay to avoid spamming
    }
}
