#ifndef POINT_SWITCHER_H_
#define POINT_SWITCHER_H_

#include <Arduino.h>
#include "MotorDriver.h"

class PointSwitcher : public MotorDriver {
public:
    PointSwitcher(uint8_t ch = 0);

    void begin();
    bool is_positive() { return is_positive_; }
    void switchPoint();

private:
    void outputSwitch();
    bool is_positive_ = false;
};

#endif //POINT_SWITCHER_H_