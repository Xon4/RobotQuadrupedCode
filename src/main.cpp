#include <Arduino.h>
#include <Servo.h>

// pin declarations


// servo object declarations
Servo FR_abad;
Servo FR_hip;
Servo FR_knee;

Servo FL_abad;
Servo FL_hip;
Servo FL_knee;

Servo BR_abad;
Servo BR_hip;
Servo BR_knee;

Servo BL_abad;
Servo BL_hip;
Servo BL_knee;

// timer declerations
unsigned long current_time = 0;
unsigned long prev_time = 0;

// mechanical constants
const int L1 = 120;
const int L2 = 120;
const int L3 = ;

//create a Leg class
class Leg {
private:
    float abad_angle, hip_angle, knee_angle;
    Servo abad_servo, hip_servo, knee_servo;

public:
    Leg(Servo abad, Servo hip, Servo knee) { 
        abad_angle = hip_angle = knee_angle = 0;
        abad_servo = abad;
        hip_servo = hip;
        knee_servo = knee;
    }

    void updateAngles(int x, int y, int z) {
        float d = sqrt(pow(x, 2) + pow(y, 2));
        float alpha = acos((pow(L1, 2) + pow(L2, 2) - pow(d, 2)) / (2 * L1 * L2));
        float theta = 90 + atan(x / y) - asin(L2 * sin(alpha) / d);
        float phi = alpha - theta;
        float row = asin(L3 / sqrt(pow(z, 2) + pow(y, 2)));
        float omega = row - atan(y / z);
        abad_angle = omega;
        hip_angle = theta;
        knee_angle = phi;
    }

    void updateServos() {
      abad_servo.write(abad_angle);
      hip_servo.write(hip_angle);
      knee_servo.write(knee_angle);
    }

    float get_abad_angle() {
      return abad_angle;
    }

    float get_hip_angle() {
      return hip_angle;
    }

    float get_knee_angle() {
      return knee_angle;
    }
};


void setup() {
 
  
}

void loop() {

}

// put function definitions here:
void trotGait(int time_delta) {
  if (current_time - prev_time > 10) {
    rate = map(rate, 0, 1023, 80 , 1);
    
    prev_time = current_time;
  }
}
