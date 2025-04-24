#include <xmiomod.h>
#include "scc_lite.h"


void (*search)(void);


void setup() {
  x_setup();
  
  pinMode(EDGE_S_L, INPUT);
  pinMode(EDGE_S_R, INPUT);
  pinMode(DIAGONALE_LEFT_S, INPUT);
  pinMode(DIAGONALE_RIGHT_S, INPUT);
  pinMode(LEFT_S, INPUT);
  pinMode(RIGHT_S, INPUT);
  pinMode(SWITCH_1,  INPUT_PULLUP);
  pinMode(SWITCH_2,  INPUT_PULLUP);
  pinMode(SWITCH_3,  INPUT_PULLUP);
  //pinMode(FRONT_S, INPUT);
  pinMode(DISTANCE_S_T, OUTPUT);
  pinMode(DISTANCE_S_E, INPUT);


  // >>>>>RULE<<<<<<
  delay(WAITING_TIME);


  backToBack = !digitalRead(SWITCH_1);
  sideAvoid = !digitalRead(SWITCH_2);
  smartPacing = !digitalRead(SWITCH_3);



  if(backToBack)
  {
    x_right0(MAX_SPEED);
    delay(180);
    x_stopMotors();
    if(sideAvoid)
      delay(100);
  }

  
  if(sideAvoid)
  {
    x_right0(MAX_SPEED);
    delay(70);
    x_forward(MAX_SPEED);
    delay(450);
    x_left0(MAX_SPEED);
    delay(140);
    x_stopMotors();
  }


  search = explorerSearch;
  searchSpeed = NORMAL_SEARCH_SPEED;
  speedIsSafe = true;
  if(smartPacing)
    increaseSearchSpeed(OPENING_SPEED, OPENING_DURATION);
    
  //Serial.begin(9600);
}


void loop() {
  debug();
  if(analogRead(EDGE_S_R) < WHITE_LINE_THRESHOLD_RIGHT || analogRead(EDGE_S_L) < WHITE_LINE_THRESHOLD_LEFT)
    edgeISR();

  if(digitalRead(DIAGONALE_RIGHT_S) && digitalRead(DIAGONALE_LEFT_S))
  {
    x_forward(MAX_SPEED);
    state = PUSHING;
  }

  else if(distance < PUSH_THRESHOLD)
  {
    x_forward(MAX_SPEED);
    state = PUSHING;
  }

  else if(distance < FOUND)
  {
    x_forward(APPROACHING_SPEED);
    state = APPROACHING;  
  }

  else if(digitalRead(RIGHT_S))
  {
    timer = 45;
    rotationStart = millis();
    rotate(RIGHT);
    state = ROTATING;
  }

  else if(digitalRead(LEFT_S))
  {
    timer = 45;
    rotationStart = millis();
    rotate(LEFT);
    state = ROTATING;
  }

  else if(digitalRead(DIAGONALE_RIGHT_S))
  {
    timer = 5;
    rotationStart = millis();
    rotate(RIGHT);
    state = ROTATING;
  }

  else if(digitalRead(DIAGONALE_LEFT_S))
  {
    timer = 5;
    rotationStart = millis();
    rotate(LEFT);
    state = ROTATING;
  }

  else if(state != ROTATING)
  {
    state = SEARCHING;
  }


  switch(state)
  {
    case ROTATING:
      if(millis() - rotationStart > timer)
      {
        x_stopMotors();
        state = OTHER;
      }
      break;

    case SEARCHING:
      if(!speedIsSafe && (millis() - searchSpeedStart > searchSpeedTimer))
        normalizeSearchSpeed();

      search();
      break;

    default: 
      state = SEARCHING;
      break;
  }

  updateDistance();
}



// ISRs
void edgeISR()
{
    x_backward(MAX_SPEED);
    delay(200);
    x_right0(MAX_SPEED);
    delay(180);
    x_stopMotors();
    if(smartPacing)
      increaseSearchSpeed(SMART_PACING_SPEED, SMART_PACING_DURATION);
    state = OTHER;
}



// smart pacing support funcs
inline void increaseSearchSpeed(uint8_t speed, uint32_t time)
{
  searchSpeedStart = millis();
  searchSpeedTimer = time;
  searchSpeed = speed;
  speedIsSafe = false;
}


inline void normalizeSearchSpeed()
{
  searchSpeed = NORMAL_SEARCH_SPEED;
  speedIsSafe = true;
}



// search functions
void explorerSearch()
{
  x_forward(searchSpeed);
}


// void radarSearch()
// {
//   if(direction)
//     x_left0(RADAR_SPEED);
//   else
//     x_right0(RADAR_SPEED);
// }


// void sweeperSearch()
// {
//   unsigned long currentTime = millis();
  
//   if (sweepState == 0) {
//     if (direction) 
//     {
//       x_left0(SWEEP_ROTATION_SPEED);
//     } 
//     else 
//     {
//       x_right0(SWEEP_ROTATION_SPEED);
//     }
    
//     if (currentTime - sweepStartTime > SWEEP_ROTATION_DURATION) {
//       sweepStartTime = currentTime;
//       sweepState = 1;
//       x_stopMotors();
//     }
//   } 
//   else {
    
//     if (currentTime - sweepStartTime > SWEEP_HOLD_DURATION) {
//       sweepStartTime = currentTime;
//       sweepState = 0;
      
//       // direction = !direction;
//     }
//   }
// }


void block_DEBUG()
{
  while(1);
}













