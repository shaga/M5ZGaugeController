#include <Arduino.h>
#include <Unit_Encoder.h>
#include "SpeedController.h"
#include <M5GFX.h>
#include <M5Unified.h>
#include "PointSwitcher.h"

#define MAX_SPEED                       (230)
#define MIN_SPEED                       (2)
#define THRESHOLD_SAFE_SPEED            (180)
#define THRESHOLD_DANGER_SPEED          (210)
#define START_SPEED                     (2)
#define TICK_LARGE                      (5)
#define TICK_SMALL                      (2)

#define SPEED_METER_W                   240
#define SPEED_METER_H                   160
#define SPEED_METER_CENTER_X            (SPEED_METER_W / 2)
#define SPEED_METER_CENTER_Y            (SPEED_METER_H / 2 + 40)
#define SPEED_METER_TEXT_BASE_X         SPEED_METER_CENTER_X
#define SPEED_METER_TEXT_BASE_Y         (SPEED_METER_H - 15)
#define SPEED_METER_RAD1                (SPEED_METER_CENTER_Y - 10)
#define SPEED_METER_RAD2                (SPEED_METER_RAD1 - 20)
#define SPEED_METER_MIN_ANGLE           (150)
#define SPEED_METER_MAX_ANGLE           (390)
#define SPEED_METER_RANGE               (SPEED_METER_MAX_ANGLE - SPEED_METER_MIN_ANGLE)
#define SPEED_METER_SPEED2ANGLE(spd)    (spd * (float)SPEED_METER_RANGE / (float)MAX_SPEED + SPEED_METER_MIN_ANGLE)
#define SPEED_METER_SAFE_LIMIT_ANGLE    SPEED_METER_SPEED2ANGLE(THRESHOLD_SAFE_SPEED)
#define SPPED_METER_DANGER_LIMIT_ANGLE  SPEED_METER_SPEED2ANGLE(THRESHOLD_DANGER_SPEED)


#define DIR_W                           (240)
#define DIR_H                           (140)
#define DIR_BASE_X                      (0)
#define DIR_BASE_Y                      (180)

#define DIR_POINT_OFFSET_X              (10)
#define DIR_EVACUATE_OFFSET_X           (30)
#define DIR_EVACUATE_OFFSET_Y           (15)
#define DIR_TEXT_BASE_X                 (DIR_W / 2 - 15)
#define DIR_TEXT_BASE_Y                 (DIR_H / 2 - 15)
#define DIR_RECT_W                      (230)
#define DIR_RECT_H                      (70)
#define DIR_RECT_R                      (DIR_RECT_H / 2)
#define DIR_RECT_X                      (DIR_W / 2 - DIR_RECT_W / 2)
#define DIR_RECT_Y                      (20)
#define DIR_ARROW_RIGHT_X1              (DIR_W / 2 + 5)
#define DIR_ARROW_RIGHT_X2              (DIR_W / 2 + 5)
#define DIR_ARROW_RIGHT_X3              (DIR_W / 2 + 22)
#define DIR_ARROW_LEFT_X1               (DIR_W / 2 - 5)
#define DIR_ARROW_LEFT_X2               (DIR_W / 2 - 5)
#define DIR_ARROW_LEFT_X3               (DIR_W / 2 - 22)
#define DIR_ARROW_TOP_Y1                (DIR_RECT_Y - 10)
#define DIR_ARROW_TOP_Y2                (DIR_RECT_Y + 10)
#define DIR_ARROW_TOP_Y3                (DIR_RECT_Y)
#define DIR_ARROW_BOTTOM_Y1             (DIR_RECT_Y + DIR_RECT_H - 10)
#define DIR_ARROW_BOTTOM_Y2             (DIR_RECT_Y + DIR_RECT_H + 10)
#define DIR_ARROW_BOTTOM_Y3             (DIR_RECT_Y + DIR_RECT_H)
#define DIR_POINT_Y                     (DIR_RECT_Y + DIR_RECT_H - 1)
#define DIR_POINT_LEFT_X                (DIR_RECT_X + DIR_RECT_R + DIR_POINT_OFFSET_X)
#define DIR_POINT_RIGHT_X               (DIR_RECT_X + DIR_RECT_W - DIR_RECT_R - DIR_POINT_OFFSET_X)
#define DIR_EVACUATE_Y                  (DIR_POINT_Y + DIR_EVACUATE_OFFSET_Y)
#define DIR_EVACUATE_LEFT_X             (DIR_POINT_LEFT_X + DIR_EVACUATE_OFFSET_X)
#define DIR_EVACUATE_RIGHT_X            (DIR_POINT_RIGHT_X - DIR_EVACUATE_OFFSET_X)

#define STATE_W                         (160)
#define STATE_H                         (50)
#define STATE_BASE_X                    (SPEED_METER_CENTER_X - STATE_W / 2)
#define STATE_BASE_Y                    (SPEED_METER_CENTER_Y - STATE_H / 2)
#define STATE_CENTER_X                  (STATE_W / 2)
#define STATE_CENTER_Y                  (STATE_H / 2)

Unit_Encoder encoder;
SpeedController speed_controller;
M5GFX display;
M5Canvas canvas_dir(&display);
M5Canvas canvas_state(&display);

PointSwitcher switcherLeft(0);
PointSwitcher switcherRight(2);

bool is_evacuate = false;
uint8_t speed = 0;
bool run_back = false;

int prev_encoder;

void drawSpeedMeter() {
  display.beginTransaction();
  //display.clear();
  float speed_angle = SPEED_METER_SPEED2ANGLE(speed);

  if (speed == 0) {
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_MIN_ANGLE, SPEED_METER_MAX_ANGLE, TFT_DARKGRAY);
  } else if (speed <= THRESHOLD_SAFE_SPEED) {
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_MIN_ANGLE, speed_angle, TFT_GREEN);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, speed_angle, SPEED_METER_MAX_ANGLE, TFT_DARKGRAY);
  } else if (speed <= THRESHOLD_DANGER_SPEED) {
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_MIN_ANGLE, SPEED_METER_SAFE_LIMIT_ANGLE, TFT_GREEN);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_SAFE_LIMIT_ANGLE, speed_angle, TFT_YELLOW);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, speed_angle, SPEED_METER_MAX_ANGLE, TFT_DARKGRAY);
  } else if (speed < MAX_SPEED) {
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_MIN_ANGLE, SPEED_METER_SAFE_LIMIT_ANGLE, TFT_GREEN);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_SAFE_LIMIT_ANGLE, SPPED_METER_DANGER_LIMIT_ANGLE, TFT_YELLOW);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPPED_METER_DANGER_LIMIT_ANGLE, speed_angle, TFT_RED);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, speed_angle, SPEED_METER_MAX_ANGLE, TFT_DARKGRAY);
  } else {
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_MIN_ANGLE, SPEED_METER_SAFE_LIMIT_ANGLE, TFT_GREEN);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPEED_METER_SAFE_LIMIT_ANGLE, SPPED_METER_DANGER_LIMIT_ANGLE, TFT_YELLOW);
    display.fillArc(SPEED_METER_CENTER_X, SPEED_METER_CENTER_Y, SPEED_METER_RAD1, SPEED_METER_RAD2, SPPED_METER_DANGER_LIMIT_ANGLE, SPEED_METER_MAX_ANGLE, TFT_RED);
  }
}

void drawDir() {
  canvas_dir.clear();
  canvas_dir.drawRoundRect(DIR_RECT_X, DIR_RECT_Y, DIR_RECT_W, DIR_RECT_H, DIR_RECT_R, TFT_WHITE);
  int bottom_offset = is_evacuate ? DIR_EVACUATE_OFFSET_Y : 0;
  if (run_back) {
    canvas_dir.drawString("外回り", DIR_TEXT_BASE_X, DIR_TEXT_BASE_Y);
    canvas_dir.fillTriangle(DIR_ARROW_RIGHT_X1, DIR_ARROW_TOP_Y1, DIR_ARROW_RIGHT_X2, DIR_ARROW_TOP_Y2, DIR_ARROW_RIGHT_X3, DIR_ARROW_TOP_Y3, TFT_WHITE);
    canvas_dir.fillTriangle(DIR_ARROW_LEFT_X1, DIR_ARROW_BOTTOM_Y1 + bottom_offset, DIR_ARROW_LEFT_X2, DIR_ARROW_BOTTOM_Y2 + bottom_offset, DIR_ARROW_LEFT_X3, DIR_ARROW_BOTTOM_Y3 + bottom_offset, TFT_WHITE);
  } else {
    canvas_dir.drawString("内回り", DIR_TEXT_BASE_X, DIR_TEXT_BASE_Y);
    canvas_dir.fillTriangle(DIR_ARROW_LEFT_X1, DIR_ARROW_TOP_Y1, DIR_ARROW_LEFT_X2, DIR_ARROW_TOP_Y2, DIR_ARROW_LEFT_X3, DIR_ARROW_TOP_Y3, TFT_WHITE);
    canvas_dir.fillTriangle(DIR_ARROW_RIGHT_X1, DIR_ARROW_BOTTOM_Y1 + bottom_offset, DIR_ARROW_RIGHT_X2, DIR_ARROW_BOTTOM_Y2 + bottom_offset, DIR_ARROW_RIGHT_X3, DIR_ARROW_BOTTOM_Y3 + bottom_offset, TFT_WHITE);
  }
  if (is_evacuate) {
    canvas_dir.drawLine(DIR_POINT_LEFT_X, DIR_POINT_Y, DIR_POINT_RIGHT_X, DIR_POINT_Y, TFT_RED);
  }
  canvas_dir.drawLine(DIR_POINT_LEFT_X, DIR_POINT_Y, DIR_EVACUATE_LEFT_X, DIR_EVACUATE_Y, is_evacuate ? TFT_WHITE : TFT_RED);
  canvas_dir.drawLine(DIR_EVACUATE_LEFT_X, DIR_EVACUATE_Y, DIR_EVACUATE_RIGHT_X, DIR_EVACUATE_Y, is_evacuate ? TFT_WHITE : TFT_RED);
  canvas_dir.drawLine(DIR_POINT_RIGHT_X, DIR_POINT_Y, DIR_EVACUATE_RIGHT_X, DIR_EVACUATE_Y, is_evacuate ? TFT_WHITE : TFT_RED);


  canvas_dir.pushSprite(DIR_BASE_X, DIR_BASE_Y);
}

void drawState() {
  canvas_state.clear();

  if (speed == 0) {
    canvas_state.setTextColor(TFT_WHITE);
    canvas_state.drawString("停車", STATE_CENTER_X, STATE_CENTER_Y);
  } else if (speed > THRESHOLD_DANGER_SPEED) {
    canvas_state.setTextColor(TFT_RED);
    canvas_state.drawString("危険速度", STATE_CENTER_X, STATE_CENTER_Y);
  } else if (speed > THRESHOLD_SAFE_SPEED) {
    canvas_state.setTextColor(TFT_YELLOW);
    canvas_state.drawString("注意速度", STATE_CENTER_X, STATE_CENTER_Y);
  } else {
    canvas_state.setTextColor(TFT_DARKGREEN);
    canvas_state.drawString("走行中", STATE_CENTER_X, STATE_CENTER_Y);
  }

  canvas_state.pushSprite(STATE_BASE_X, STATE_BASE_Y);
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  display.begin();
  display.setRotation(0);

  canvas_dir.createSprite(DIR_W, DIR_H);
  canvas_dir.setTextDatum(middle_center);
  canvas_dir.setTextColor(TFT_WHITE);
  canvas_dir.setFont(&fonts::lgfxJapanGothic_40);

  canvas_state.createSprite(STATE_W, STATE_H);
  canvas_state.setTextDatum(middle_center);
  canvas_state.setFont(&fonts::lgfxJapanGothic_40);

  encoder.begin();
  prev_encoder = encoder.getEncoderValue();
  speed_controller.begin();
  switcherLeft.begin();

  Serial.begin(115200);

  drawSpeedMeter();
  drawDir();
  drawState();
}

void loop() {
  M5.update();
  static int before_button = 0;

  int current_button = encoder.getButtonStatus();

  if (speed == 0 && M5.BtnC.wasPressed()) {
    switcherLeft.switchPoint();
    switcherRight.switchPoint();
    is_evacuate = switcherLeft.is_positive() && switcherRight.is_positive();
    drawDir();
  } else if ((before_button == 0 && current_button == 1) || M5.BtnB.wasPressed()) {
    if (speed == 0) {
      speed_controller.switchDirection();
      run_back = !run_back;
      drawDir();
    } else {
      speed_controller.setSpeed(0);
      speed = 0;
      drawSpeedMeter();
      drawState();
    }
  } else {
    int new_speed = speed;
    int current_encoder = encoder.getEncoderValue();

    int diff = current_encoder - prev_encoder;

    new_speed += diff;
    if (new_speed > MAX_SPEED) new_speed = MAX_SPEED;
    else if (new_speed < 0) new_speed = 0;

    if (speed != new_speed) {
      speed = (uint8_t) new_speed;
      drawSpeedMeter();
      drawState();
      speed_controller.setSpeed(speed);
    }

    prev_encoder = current_encoder;
  }

  before_button = current_button;
}
