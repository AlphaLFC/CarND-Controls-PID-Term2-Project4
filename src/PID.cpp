#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {
  Kp = 0;
  Ki = 0;
  Kd = 0;
  p_error = 0;
  i_error = 0;
  d_error = 0;
}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  PID::Kp = Kp;
  PID::Ki = Ki;
  PID::Kd = Kd;
}

void PID::UpdateError(double cte) {
  // previous cte is the initial p_error
  double prev_cte = p_error;
  // update P,I,D error according to the current cte
  p_error = cte;
  i_error += cte;
  d_error = cte - prev_cte;
}

double PID::TotalError() {
  return -(Kp * p_error + Ki * i_error + Kd * d_error);
}

