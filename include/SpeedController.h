#ifndef SPEED_CONTROLLER_H_
#define SPEED_CONTROLLER_H_

#include <Arduino.h>
#include "MotorDriver.h"

class SpeedController : public MotorDriver {
public:
    SpeedController(uint8_t channel = 3);
    void begin();
    void switchDirection();
    void setSpeed(uint8_t speed);
    void stop();

    bool run_back();
    bool is_running();

private:
    uint8_t speed_;
    bool run_back_;
};

#endif //SPEED_CONTROLLER_H_