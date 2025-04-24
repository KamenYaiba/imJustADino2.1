#include "Arduino.h"
#ifndef SCC_LITE_H
#define SCC_LITE_H

#include <xmiomod.h>


//sensors
#define EDGE_S_R A4
#define EDGE_S_L A5
#define LEFT_S 0
#define RIGHT_S 1
#define SWITCH_1 5
#define SWITCH_2 6
#define SWITCH_3 7
#define FRONT_S A2
#define DISTANCE_S_T 2
#define DISTANCE_S_E A1
#define DIAGONALE_RIGHT_S 3
#define DIAGONALE_LEFT_S 4


// directions
#define RIGHT 0
#define LEFT 1


// thresholds
#define DS_TIMEOUT 40000
#define PUSH_THRESHOLD 950
#define FOUND 5000
#define NOT_FOUND 32767
#define WHITE_LINE_THRESHOLD_LEFT 85
#define WHITE_LINE_THRESHOLD_RIGHT 90



// timers
#define WAITING_TIME 5000
#define SWEEP_HOLD_DURATION 200
#define SWEEP_ROTATION_DURATION 100
#define DISTANCE_COOLDOWN 5
#define OPENING_DURATION 350
#define SMART_PACING_DURATION 200

// speeds
#define MAX_SPEED 255
#define RADAR_SPEED 60
#define NORMAL_SEARCH_SPEED 80
#define APPROACHING_SPEED 180
#define SWEEP_ROTATION_SPEED 100
#define OPENING_SPEED 255
#define SMART_PACING_SPEED 180



// DIPswitches
uint8_t backToBack;
uint8_t sideAvoid;
uint8_t smartPacing;

// globals
uint8_t searchSpeed;
uint64_t searchSpeedStart;
uint32_t searchSpeedTimer;
uint8_t speedIsSafe;
uint64_t distanceLastUpdate;
uint64_t rotationStart;
uint64_t timer;
bool safetyON = true;
uint32_t distance = NOT_FOUND;
uint8_t direction = RIGHT;
unsigned long sweepStartTime = 0;
uint8_t sweepState = 0;// 0: rotating, 1: holding



enum State {
  OTHER,
  SEARCHING,
  APPROACHING,
  PUSHING,
  ROTATING,
  SIDE_ATTACKING,
  EVADING
};

char *snames[] = {
  "OTHER",
  "SEARCHING",
  "APPROACHING",
  "PUSHING",
  "ROTATING",
  "SIDE_ATTACKING",
  "EVADING"
  };


State state = OTHER;
State prevState = OTHER;


//helpers
inline unsigned long getTimeElapsed() {
    return millis() - distanceLastUpdate;
}


void updateDistance() {
  if(getTimeElapsed() < DISTANCE_COOLDOWN)
    return;

  digitalWrite(DISTANCE_S_T, LOW);
  delayMicroseconds(2);
  
  digitalWrite(DISTANCE_S_T, HIGH);
  delayMicroseconds(10);
  digitalWrite(DISTANCE_S_T, LOW);
  
  // 58.3 us = 1 cm
  int tempDistance = pulseIn(DISTANCE_S_E, HIGH, DS_TIMEOUT);
  if(tempDistance)
    distance = tempDistance;
  else 
    distance = NOT_FOUND;

  distanceLastUpdate = millis();
} 


void rotate(uint8_t direction) {
  
  if(direction)
    x_left0(MAX_SPEED);
  else
    x_right0(MAX_SPEED);
}


void debugDelta() {
  char buffer[150];
  snprintf(buffer, sizeof(buffer),
           "%s | "
           "Right Speed: %3d | "
           "Left Speed %3d |"
           "Search IR: %d | "
           "Edge Left: %d | "
           "Edge Right: %d | "
           "Diagnale Left IR: %d | "
           "Diagnale Right IR: %d | "
           "Right IR: %d | "
           "Ultrasonic Sensor: %4d",
           snames[state],
           x_rightMotorSpeed_DEBUG(),
           x_leftMotorSpeed_DEBUG(),
           digitalRead(FRONT_S), 
           digitalRead(EDGE_S_L), 
           digitalRead(EDGE_S_R), 
           digitalRead(DIAGONALE_LEFT_S),
           digitalRead(DIAGONALE_RIGHT_S),
           digitalRead(LEFT_S), 
           digitalRead(RIGHT_S), 
           distance);
           
  Serial.println(buffer);
}


void debug() {
  delay(5);
  static int minLeft = 1000;
  static int minRight = 1000;
  int LR = analogRead(EDGE_S_L);
  int RR = analogRead(EDGE_S_R);
  minLeft = min(LR, minLeft);

  minRight = min(RR, minRight);

  updateDistance();
  char buffer[300];
  snprintf(buffer, sizeof(buffer),
           "State: %s\n"
           "Right Speed: %d\n"
           "Left Speed %d\n"
           "Search IR: %d\n"
           "Edge Left: %d\n"
           "Edge Right: %d\n"
           "Left IR: %d\n"
           "Right IR: %d\n"
           "Ultrasonic Sensor: %d\n"
           "MIN LEFT: %d\n"
           "MIN RIGHT: %d\n"
           "D left: %d\n"
           "D right: %d\n",
           snames[state],
           x_rightMotorSpeed_DEBUG(),
           x_leftMotorSpeed_DEBUG(),
           digitalRead(FRONT_S), 
           analogRead(EDGE_S_L), 
           analogRead(EDGE_S_R), 
           digitalRead(LEFT_S), 
           digitalRead(RIGHT_S), 
           distance,
           minLeft,
           minRight,
           digitalRead(FRONT_S),
           digitalRead(DIAGONALE_RIGHT_S));
  Serial.println(buffer);
}


void getFrequency() {
  static long counter = 0;
  counter++;
  if(millis() > 10000) {
    Serial.println(counter);
  }
}


#endif