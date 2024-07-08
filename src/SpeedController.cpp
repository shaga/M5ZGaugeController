#include <Wire.h>
#include "SpeedController.h"

SpeedController::SpeedController(uint8_t channel) : MotorDriver(channel), speed_(0), run_back_(false) {
}

void SpeedController::begin() {
    //Wire1.begin(21, 22);
}

bool SpeedController::run_back() {
    return run_back_;
}

bool SpeedController::is_running() {
    return speed_ > 0;
}

void SpeedController::switchDirection() {
    if (is_running()) return;

    run_back_ = !run_back_;
}

void SpeedController::setSpeed(uint8_t speed) {
    speed_ = speed;

    int16_t value = speed * (run_back_ ? -1 : 1);

    setPwm(value);
}

void SpeedController::stop() {
    setSpeed(0);
}