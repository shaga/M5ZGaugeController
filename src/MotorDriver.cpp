#include "MotorDriver.h"
#include <Wire.h>
 
const uint8_t MotorDriver::DRIVER_ADDR = 0x56;

MotorDriver::MotorDriver(uint8_t channel) {
    if (channel > 3) channel = 0;
    channel_ = channel;
}

void MotorDriver::setPwm(int16_t pwm) {
    Serial.printf("channel: %d pwm: %d\n", channel_, pwm);
    Wire.beginTransmission(DRIVER_ADDR);
    Wire.write(channel_ * 2);
    Wire.write((uint8_t *)&pwm, 2);
    Wire.endTransmission();
}