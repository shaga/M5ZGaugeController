#include "PointSwitcher.h"

PointSwitcher::PointSwitcher(uint8_t channel) : MotorDriver(channel), is_positive_(false) {

}

void PointSwitcher::begin() {
    outputSwitch();
}

void PointSwitcher::switchPoint() {
    is_positive_ = !is_positive_;

    outputSwitch();
}

void PointSwitcher::outputSwitch() {
    int16_t pwm = is_positive_ ? 255 : -255;

    setPwm(pwm);
    delay(50);
    setPwm(0);
}