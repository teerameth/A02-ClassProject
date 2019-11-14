#include <mbed.h>

Serial PC(USBTX, USBRX), bluetooth(D8, D2);//Bluetooth (RX, TX)
uint8_t receive = 0;
char buffer[9];
char buffer1[5];
uint16_t data[4] = {10, 20, 30, 40};
uint8_t data1[2];
PwmOut Servo(D9), Propeller(D10);
int highByte, lowByte;
int currentDegree = 90;
int delayms = 20;

void split(int n) {
    highByte = (n / 128)%128 + 1;
    lowByte = n % 128 + 1;
}

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

void onBluetoothReceived(void){
  if(bluetooth.readable()){
    bluetooth.gets(buffer1, sizeof(buffer1));
    for(int j=0; j < sizeof(data); j++){
      data1[j] = (buffer1[j*2]-1)*128 + (buffer1[j*2+1]-1);
      // PC.printf("data[%d] = %d\n", j, data[j]);
    }
  }
}

int main() {
  PC.baud(38400);
  bluetooth.baud(38400);
  Servo.period(0.02f);
  Propeller.period(0.02f);
  float degree = 0;
  int PWM = 0;
  Timer T;
  T.start();
  bluetooth.attach(&onBluetoothReceived, Serial::RxIrq);
  PC.printf("Slave\n");
  while(1) {
    if(T.read() > 0.2){
      T.stop();
      T.reset();
      T.start();
      for(int i=0;i<4;i++){
        split(data[i]);
        buffer[i*2] = highByte;
        buffer[i*2+1] = lowByte;
      }
      bluetooth.puts(buffer);

    }
  }
}