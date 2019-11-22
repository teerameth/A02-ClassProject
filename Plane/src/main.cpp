#include <mbed.h>
#include "DHT22.h"
Serial PC(USBTX, USBRX), bluetooth(D8, D2);//Bluetooth (RX, TX)
uint8_t receive = 0;
char recieve_buffer[9];
char send_buffer[5];
uint32_t data[4] = {0, 0, 0, 0};
uint32_t data1[2];
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
  // PC.printf("%f\n", PWM);
  Propeller.write(PWM);
}

void BluetoothReceived(void){
    bluetooth.gets(send_buffer, sizeof(send_buffer));
    for(int j=0; j < 2; j++){
      if(send_buffer[j*2] > 0 && send_buffer[j*2+1] > 0){
        data1[j] = (send_buffer[j*2]-1)*128 + (send_buffer[j*2+1]-1);
        PC.printf("data[%d] = %d\n", j, data1[j]);
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
      setPropeller(float(data1[1])/1024/10);//PWM //limited at 10%
    }

    if((sendable && T.read() > 0.2)){
      T.stop();
      T.reset();
      T.start();
      data[0] = int(data1[1]); //speed
      data[1] = int(currentDegree); //roll
      dht22.readData();
      data[2] = int(dht22.ReadTemperature());
      data[3] = int(dht22.ReadHumidity());

      for(int i=0;i<4;i++){
        split(data[i]);
        recieve_buffer[i*2] = highByte;
        recieve_buffer[i*2+1] = lowByte;
      }
      bluetooth.puts(recieve_buffer);
      sendable = false;
    }
    
  }
}