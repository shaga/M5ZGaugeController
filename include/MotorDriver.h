#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_

#include <Arduino.h>

class MotorDriver {
public:
    MotorDriver(uint8_t channel);

protected:
    static const uint8_t DRIVER_ADDR;
    void setPwm(int16_t pwm);
    uint8_t channel_;
};


#endif //MOTOR_DRIVER_H_
