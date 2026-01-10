#include "AHRS.h"

AHRS::AHRS() : initialized(false), isStatic(true), stationaryStartTime(0)
{
    mpu = new MPU9250();
    for (int i = 0; i < 3; i++)
        velocity[i] = position[i] = linearAccel[i] = 0;
}

AHRS::~AHRS()
{
    delete mpu;
}

bool AHRS::begin()
{
    Wire.begin();
    if (!mpu->setup(0x68))
        return false;

    initialized = true;
    lastUpdateTime = millis();
    return true;
}

void AHRS::calibrate()
{
    if (!initialized)
        return;
    mpu->calibrateAccelGyro();
    mpu->calibrateMag();
}

// Rotate vector from body frame to world frame using quaternion
// v_world = q * v_body * q_conjugate
void AHRS::rotateVector(float *q, float *v, float *out)
{
    float qw = q[0], qx = q[1], qy = q[2], qz = q[3];

    float xx = qx * qx, yy = qy * qy, zz = qz * qz;
    float xy = qx * qy, xz = qx * qz, yz = qy * qz;
    float wx = qw * qx, wy = qw * qy, wz = qw * qz;

    out[0] = (1 - 2 * (yy + zz)) * v[0] + 2 * (xy - wz) * v[1] + 2 * (xz + wy) * v[2];
    out[1] = 2 * (xy + wz) * v[0] + (1 - 2 * (xx + zz)) * v[1] + 2 * (yz - wx) * v[2];
    out[2] = 2 * (xz - wy) * v[0] + 2 * (yz + wx) * v[1] + (1 - 2 * (xx + yy)) * v[2];
}

void AHRS::update()
{
    if (!initialized || !mpu->update())
        return;

    unsigned long now = millis();
    float dt = (now - lastUpdateTime) * 0.001f;
    lastUpdateTime = now;

    if (dt <= 0 || dt > 0.1f)
        return;

    // Get raw accelerometer in body frame (units: g, convert to m/s²)
    float AccX = mpu->getAccX() * G_CONST;
    float AccY = mpu->getAccY() * G_CONST;
    float AccZ = mpu->getAccZ() * G_CONST;

    // Get orientation angles in radians
    // AngleX = Roll, AngleY = Pitch, AngleZ = Yaw
    float AngleXr = mpu->getRoll() * DEG_TO_RAD;
    float AngleYr = mpu->getPitch() * DEG_TO_RAD;
    float AngleZr = mpu->getYaw() * DEG_TO_RAD;


    float phi   = AngleXr;           // roll
    float theta = AngleYr;           // pitch
    float psi   = AngleZr;           // yaw
    float cphi = cos(phi), sphi = sin(phi);
    float cth = cos(theta), sth = sin(theta);
    float cpsi = cos(psi), spsi = sin(psi);

    // R =  Rx(phi) * Ry(theta) * Rz(psi)
    float R00 = cpsi * cth;
    float R01 = -(cth * spsi);
    float R02 = sth;

    float R10 = (sth * sphi * cpsi) + (cphi * cpsi);
    float R11 = -(spsi * sth * sphi) + (cpsi * cphi);
    float R12 = -(sphi * cth);

    float R20 = -(sth * sphi * cpsi) + (sphi * spsi);
    float R21 = (sth * sphi * spsi) + (sphi * cpsi);
    float R22 = cth * cphi;

    // ---- ***************** ----
    // R = Rz(psi) * Ry(theta) * Rx(phi)
    // float R00 = cpsi * cth;
    // float R01 = -(spsi * cphi)  + (cpsi * sth * sphi);
    // float R02 = cpsi * sth * cphi + spsi * sphi;

    // float R10 = spsi * cth;
    // float R11 = (spsi * sth * sphi) + cpsi * cphi;
    // float R12 = (spsi * sth * cphi) - cpsi * sphi;

    // float R20 = -sth;
    // float R21 = cth * sphi;
    // float R22 = cth * cphi;
    // ----------------------
    // float R00 = cpsi * cth;
    // float R01 = cth * spsi;
    // float R02 = -sth;

    // float R10 = (sth * sphi * cpsi) - (cphi * spsi);
    // float R11 = (spsi * sth * sphi) + cpsi * cphi;
    // float R12 = sphi * cth;

    // float R20 = (cphi * sth * cpsi ) + (sphi * spsi);
    // float R21 = (cphi * sth * cpsi) - (sphi *spsi);
    // float R22 = cth * cphi;



    // --------********----------

    // float RealAccX = (cphi * AccX) + (sth * AccY) + (spsi * AccZ);
    // float RealAccY = (sphi * AccX) + (cth * AccY) + (spsi * AccZ);
    // float RealAccZ = (sphi * AccX) + (sth * AccY) + (cpsi * AccZ);

    // // Transform acceleration from body frame to world frame using ZYX rotation
    // // ((ZY)X) rotation matrix applied to acceleration vector
    // float RealAccX = cos(AngleZr) * (AccX * cos(AngleYr) + sin(AngleYr) * (AccY * sin(AngleXr) + AccZ * cos(AngleXr))) + sin(AngleZr) * (AccZ * sin(AngleXr) - AccY * cos(AngleXr));
    // float RealAccY = sin(AngleZr) * (AccX * cos(AngleYr) + sin(AngleYr) * (AccY * sin(AngleXr) + AccZ * cos(AngleXr))) + cos(AngleZr) * (AccY * cos(AngleXr) - AccZ * sin(AngleXr));
    // float RealAccZ = cos(AngleYr) * (AccY * sin(AngleXr) + AccZ * cos(AngleXr)) - AccX * sin(AngleYr);

    // Remove gravity from Z axis (in world frame, gravity is always -9.81 on Z)
    // RealAccZ = RealAccZ - G_CONST;

    // float RealAccX = R00 * AccX + R10 * AccY + R20 * AccZ;
    // float RealAccY = R01 * AccX + R11 * AccY + R21 * AccZ;
    // float RealAccZ = R02 * AccX + R12 * AccY + R22 * AccZ;

    // RealAccZ -= G_CONST;

    // // Apply low-pass filter to reduce noise
    // const float alpha = 0.1f;
    // linearAccel[0] = alpha * RealAccX + (1.0f - alpha) * linearAccel[0];
    // linearAccel[1] = alpha * RealAccY + (1.0f - alpha) * linearAccel[1];
    // linearAccel[2] = alpha * RealAccZ + (1.0f - alpha) * linearAccel[2];


    linearAccel[0] = mpu->getLinearAccX();
    linearAccel[1] = mpu->getLinearAccY();
    linearAccel[2] = mpu->getLinearAccZ();

    // Calculate magnitudes for motion detection
    float accNorm = sqrt(
        linearAccel[0] * linearAccel[0] +
        linearAccel[1] * linearAccel[1] +
        linearAccel[2] * linearAccel[2]);
    

    float gyroNorm = sqrt(
        sq(mpu->getGyroX()) +
        sq(mpu->getGyroY()) +
        sq(mpu->getGyroZ()));

    // ZUPT: Zero Velocity Update
    if (accNorm < MOTION_THRESHOLD && gyroNorm < GYRO_THRESHOLD)
    {
        if (stationaryStartTime == 0)
        {
            stationaryStartTime = now;
        }

        // If stationary for sufficient time, zero velocity
        if (now - stationaryStartTime > STATIONARY_TIME_MS)
        {
            isStatic = true;
            velocity[0] = velocity[1] = velocity[2] = 0;
            return;
        }
    }
    else
    {
        stationaryStartTime = 0;
        isStatic = false;
    }

    // 6️⃣ Integration (only when moving)
    if (!isStatic)
    {
        for (int i = 0; i < 3; i++)
        {
            velocity[i] += linearAccel[i] * dt;
            position[i] += velocity[i] * dt;
        }
    }
}

bool AHRS::isMoving()
{
    return !isStatic;
}

void AHRS::resetPosition()
{
    for (int i = 0; i < 3; i++)
        velocity[i] = position[i] = 0;
}
