#include <mbed.h>
#include "DHT22.h"
Serial PC(USBTX, USBRX), bluetooth(D8, D2);//Bluetooth (RX, TX)
uint8_t receive = 0;
char buffer[9];
char buffer1[5];
uint16_t data[4] = {1000, 2000, 3000, 4000};
uint16_t data1[2];
PwmOut Servo(D9), Propeller(D10);
int highByte, lowByte;
int currentDegree = 90;
int delayms = 20;
bool sendable=false;

DHT22 dht22(D4);

void split(int n) {
    highByte = (n / 128) + 1;
    lowByte = n % 128 + 1;
}

void setServo(int degree){
    currentDegree = degree;
    float duty = ((float(degree) / 180.0) * 0.05) + 0.05;
    Servo.write(duty);
}

void setPropeller(float PWM){
  int min=40, max=500;
  // PWM = (PWM *(max-min)/1024) + min;
  PC.printf("%f\n", PWM);
  Propeller.write(PWM);
}

void BluetoothReceived(void){
    bluetooth.gets(buffer1, sizeof(buffer1));
    for(int j=0; j < 2; j++){
      if(buffer1[j*2] > 0 && buffer1[j*2+1] > 0){
        data1[j] = (buffer1[j*2]-1)*128 + (buffer1[j*2+1]-1);
        // PC.printf("data[%d] = %d\n", j, data1[j]);
        // PC.printf("%d, %d\n", buffer1[j*2]-1, buffer1[j*2+1]);
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
  Timer T1;
  T1.start();
  T.start();
  PC.printf("Slave\n");
  while(1) {
    if(bluetooth.readable()){
      BluetoothReceived();
      sendable = true;
    }
    degree = float(data1[0]) * 180 / 1024;
    if(currentDegree < degree){//Servo
      currentDegree += 1;
      setServo(currentDegree);
    }
    else if(currentDegree > degree){
      currentDegree -= 1;
      setServo(currentDegree);
    }
    if(T1.read() > 0.1){
      T1.stop();T1.start();
      setPropeller(float(data1[1])/1024);//PWM
    }

    if((sendable && T.read() > 0.2)){
      T.stop();
      T.reset();
      T.start();

      dht22.readData();
      data[2] = int(dht22.ReadTemperature());
      data[3] = int(dht22.ReadHumidity());

      for(int i=0;i<4;i++){
        split(data[i]);
        buffer[i*2] = highByte;
        buffer[i*2+1] = lowByte;
      }
      bluetooth.puts(buffer);
      sendable = false;
      // PC.printf("%d\n", buffer);
    }
    
  }
}