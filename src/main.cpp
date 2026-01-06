#include <Arduino.h>
#include <Display.h>
#include <AHRS.h>
#include <ServoControl.h>
#include <Network.h>
#include <Training.h>
#include <HealthCheck.h>

// Pin definitions
const uint8_t SERVO_PIN_DOWN = 16; // P1 on board
const uint8_t SERVO_PIN_UP = 15;   // P2 on board

// Global objects
Display display;
AHRS ahrs;
ServoControl servoControl(SERVO_PIN_DOWN, SERVO_PIN_UP);
Network *network; // Heap allocation needed for initialization order with display
Training training;
HealthCheck healthCheck(&display, &ahrs, &servoControl);

void setup()
{
    Serial.begin(115200);
    // Initialize Display
    display.begin();
    delay(2000);

    display.clear();
    display.print("RL Robot V2", 0, 0);
    display.setCursor(0, 16);
    display.print("Initializing...");
    // Initialize Network
    network = new Network(&display);
    network->begin();
    network->startOTATask();

    // Get robot number 
    display.setCursor(0, 32);
    display.print("Getting Robot Number from EEPROM...");
    int robotNum = network->getRobotNumber();
    
    // Display robot info
    display.clear();
    display.print("Robot #", 0, 0);
    display.print(robotNum, 60, 0);
    display.setCursor(0, 16);
    display.print("Setup...");

    // Initialize AHRS
    display.clear();
    display.print("Init AHRS...", 0, 0);
    if (ahrs.begin())
    {
        display.setCursor(0, 16);
        display.print("AHRS OK");
    }
    else
    {
        display.setCursor(0, 16);
        display.print("AHRS Failed!");
    }
    delay(1000);

    // Initialize Servos
    display.clear();
    display.print("Init Servos...", 0, 0);
    servoControl.begin();
    display.setCursor(0, 16);
    display.print("Servos OK");
    delay(500);

    // Setup complete
    display.clear();
    display.print("Setup Complete", 0, 0);
    delay(500);

    // Calibrate AHRS
    display.clear();
    display.print("Calibrating AHRS", 0, 0);
    ahrs.calibrateAccelGyro();
    display.setCursor(0, 16);
    display.print("Calibration Done");
    delay(300);

    // Run health check
    healthCheck.run();

    // Reset measurement for interval tracking
    ahrs.resetMeasurement();
}

void loop()
{
    ahrs.update();

    static unsigned long lastMeasurement = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastMeasurement >= 2000)
    { // Every 2 seconds
        lastMeasurement = currentTime;

        // Get measurement data
        AHRS::MovementSnapshot measurement = ahrs.getMeasurement();

        // Display the interval data
        display.clear();

        // Line 1: Distance moved in interval (cm)
        display.setCursor(0, 0);
        display.print("Dist: ");
        display.print(measurement.deltaDistance);
        display.print(" cm");

        // Line 2: Average speed in interval (cm/s)
        display.setCursor(0, 12);
        display.print("Spd: ");
        display.print(measurement.avgSpeed);
        display.print(" cm/s");

        // Line 3: Average acceleration in interval (m/s^2)
        display.setCursor(0, 24);
        display.print("Acc: ");
        display.print(measurement.avgAcceleration);
        display.print(" m/s2");

        // Line 4: Time interval
        display.setCursor(0, 36);
        display.print("Time: ");
        display.print(measurement.deltaTime);
        display.print(" s");
        

        display.refresh();

        // Reset for next measurement
        ahrs.resetMeasurement();

        // Print to serial for debugging
        Serial.print("Distance: ");
        Serial.print(measurement.deltaDistance);
        Serial.print(" cm, Speed: ");
        Serial.print(measurement.avgSpeed);
        Serial.print(" cm/s, Accel: ");
        Serial.print(measurement.avgAcceleration);
        Serial.println(" m/s2");
        // add serial monitoring also for these data ax, ay, az (همون getAccX/Y/Z)roll, pitch, yaw accMag = sqrt(ax ^ 2 + ay ^ 2 + az ^ 2) dt velocityX, Y, Z و displacementX, Y, Z
        Serial.print("Accel X: ");
        Serial.print(ahrs.getAccelX());
        Serial.print(" m/s2, Y: ");
        Serial.print(ahrs.getAccelY());
        Serial.print(" m/s2, Z: ");
        Serial.print(ahrs.getAccelZ());
        Serial.print(" m/s2, Accel Mag: ");
        Serial.print(ahrs.getAccelMagnitude());
        Serial.println(" m/s2");
        Serial.print("Roll: ");
        Serial.print(ahrs.getRoll());
        Serial.print(" deg, Pitch: ");
        Serial.print(ahrs.getPitch());
        Serial.print(" deg, Yaw: ");
        Serial.print(ahrs.getYaw());
        Serial.println(" deg");


        // Temperature
        Serial.print("Temp: ");
        Serial.print(ahrs.getTemperature());
        Serial.println(" C");
    }

    // TODO: Implement main loop logic
    // - Read sensors
    // - Process data
    // - Execute training if active
    // - Execute learned behavior
    // - Control servos
}
