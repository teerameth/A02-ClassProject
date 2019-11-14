#include <mbed.h>
#include <string>
AnalogIn X(A0), Y(A1);
DigitalIn A(D3), B(D4);
PwmOut Servo(D9), Propeller(D10);
float buffer = 0.1;
int currentDegree = 90;
int delayms = 20;

void setServo(int degree){
    currentDegree = degree;
    float duty = ((float(degree) / 180.0) * 0.05) + 0.05;
    Servo.write(duty);
}

void slowlySetServo(int degree){
  while(1){
    if(currentDegree < degree){
      currentDegree += 1;
      setServo(currentDegree);
      wait_ms(delayms);
    }
    else if(currentDegree > degree){
      currentDegree -= 1;
      setServo(currentDegree);
      wait_ms(delayms);
    }
    else {
      break;
    }
  }
}

void setPropeller(int PWM){
  Propeller.write(PWM);
}

int main() {
  Servo.period(0.02f);
  Propeller.period(0.02f);
  float degree = 0;
  int PWM = 0;
  while(1) {
    int lastStateA = A.read(); 
    int lastStateB = B.read();
    wait_ms(10);
    if(lastStateA == 0 && A.read() == 1){
      // aom function to display tmp humid
    }
    if(lastStateB == 0 && B.read() == 1){
      // aom function to display speed and angle
    }

    degree = X.read() * 180;
    PWM = Y.read();
    slowlySetServo(degree);
    setPropeller(PWM);
  }
}