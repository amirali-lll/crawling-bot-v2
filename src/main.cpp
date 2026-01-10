#include <Arduino.h>
#include <Display.h>
#include <AHRS.h>
#include <ServoControl.h>
#include <Network.h>
#include <Training.h>
#include <HealthCheck.h>

// Pin definitions
const uint8_t SERVO_PIN_DOWN = 16;
const uint8_t SERVO_PIN_UP = 15;

// Global objects
Display display;
AHRS ahrs;
ServoControl servoControl(SERVO_PIN_DOWN, SERVO_PIN_UP);
Network *network;
Training training;
HealthCheck healthCheck(&display, &ahrs, &servoControl);

// متغیرهای کمکی برای محاسبه بازه‌ای (Interval Calculation)
float lastPosX = 0, lastPosY = 0, lastPosZ = 0;
unsigned long lastLogTime = 0;

void setup()
{
    Serial.begin(115200);

    // 1. Initialize Display
    display.begin();
    delay(1000);
    display.clear();
    display.print("RL Robot V2", 0, 0);
    display.print("Booting...", 0, 16);

    // 2. Initialize Network
    network = new Network(&display);
    network->begin();
    network->startOTATask();

    // 3. Get Robot ID
    int robotNum = network->getRobotNumber();
    display.clear();
    display.print("Robot ID: ", 0, 0);
    display.print(robotNum, 60, 0);

    // 4. Initialize AHRS
    display.print("Init IMU...", 0, 16);
    if (ahrs.begin())
    {
        Serial.println("AHRS Initialized");
        display.print("OK", 80, 16);
    }
    else
    {
        Serial.println("AHRS Failed");
        display.print("FAIL", 80, 16);
        while (1)
            ; // Stop if sensor fails
    }
    delay(500);

    // 5. Initialize Servos
    servoControl.begin();

    // 6. Calibrate
    display.clear();
    display.print("Calibrating...", 0, 0);
    display.print("Keep Still!", 0, 16);
    Serial.println("Calibrating...");

    // تابع جدید کالیبراسیون که در کد قبلی نوشتم
    ahrs.calibrate();

    display.clear();
    display.print("Ready!", 0, 0);
    delay(500);

    // Health Check
    healthCheck.run();

    // تنظیم زمان اولیه
    lastLogTime = millis();
}

void loop()
{
    ahrs.update();

    unsigned long currentTime = millis();
    float dt_log = (currentTime - lastLogTime) / 1000.0;

    if (dt_log >= 0.5)
    {
        lastLogTime = currentTime;

        float velX = ahrs.getVelocityX();
        float velY = ahrs.getVelocityY();
        float velZ = ahrs.getVelocityZ();

        float posX = ahrs.getPositionX();
        float posY = ahrs.getPositionY();
        float posZ = ahrs.getPositionZ();

        float linAccX = ahrs.getLinearAccelX();
        float linAccY = ahrs.getLinearAccelY();
        float linAccZ = ahrs.getLinearAccelZ();

        float speed = ahrs.getSpeed();

        float dX = posX - lastPosX;
        float dY = posY - lastPosY;
        float dZ = posZ - lastPosZ;
        float distInInterval = sqrt(dX * dX + dY * dY + dZ * dZ);

        // آپدیت موقعیت قبلی برای دور بعد
        lastPosX = posX;
        lastPosY = posY;
        lastPosZ = posZ;

        // --- نمایش روی LCD ---
        display.clear();

        // خط 1: وضعیت حرکت و سرعت
        display.setCursor(0, 0);
        display.print(ahrs.isMoving() ? "MOVING" : "STATIC");
        display.print(" V:");
        display.print(speed * 100, 0); // نمایش سرعت به cm/s
        display.print("cm/s");

        // خط 2: موقعیت X و Y (به سانتیمتر)
        display.setCursor(0, 16);
        display.print("X:");
        display.print(posX * 100, 0);
        display.print(" Y:");
        display.print(posY * 100, 0);

        // خط 3: موقعیت Z و مسافت فاصله
        display.setCursor(0, 32);
        display.print("Z:");
        display.print(posZ * 100, 0);
        display.print(" D:");
        display.print(distInInterval * 100, 1);

        display.refresh();

        // --- نمایش دقیق در Serial Monitor (برای دیباگ) ---
        // فرمت CSV برای خوانایی بهتر یا رسم نمودار
        Serial.print("Stat:");
        Serial.print(ahrs.isMoving() ? "MOV" : "STP");

        Serial.print(" | Pos(m) X:");
        Serial.print(posX, 3);
        Serial.print(" Y:");
        Serial.print(posY, 3);
        Serial.print(" Z:");
        Serial.print(posZ, 3);

        Serial.print(" | Vel(m/s) X:");
        Serial.print(velX, 3);
        Serial.print(" Y:");
        Serial.print(velY, 3);
        Serial.print(" Z:");
        Serial.print(velZ, 3);

        Serial.print(" | Speed:");
        Serial.print(speed, 3);
        
        Serial.print(" | LinAcc(m/s²) X:");
        Serial.print(linAccX, 2);
        Serial.print(" Y:");
        Serial.print(linAccY, 2);
        Serial.print(" Z:");
        Serial.print(linAccZ, 2);

        Serial.print(" | ΔDist(cm):");
        Serial.print(distInInterval * 100, 2);

        Serial.print(" | Orient R:");
        Serial.print(ahrs.getRoll(), 1);
        Serial.print(" P:");
        Serial.print(ahrs.getPitch(), 1);
        Serial.print(" Y:");
        Serial.print(ahrs.getYaw(), 1);

        Serial.println();
        }

    // ================================================================
    // 3. MAIN ROBOT LOGIC
    // ================================================================
    // اینجا کدهای مربوط به تصمیم‌گیری ربات و حرکت سروو‌ها قرار می‌گیرد
}