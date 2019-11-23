#include <mbed.h>
#include "DHT22.h"
Serial PC(USBTX, USBRX, 38400), bluetooth(D8, D2, 38400);//Bluetooth (RX, TX)
char get_buffer[11], send_buffer[11];//"0000,0000;" -> 11
PwmOut Servo(D9), Propeller(D10);
int currentDegree = 90, currentSpeed = 512;
int delayms = 20;
bool sendable=false;

DHT22 dht22(D4);

int16_t data[4] = {50, 0, 25, 50};//Speed, Roll, Temp, Humid
char* token;
char buffer_T[5], buffer_H[5];

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
    bluetooth.gets(get_buffer, 11);
    if (get_buffer[4] == ',' && get_buffer[9] == ';'){
      PC.printf("get_buffer = [%s]\n", get_buffer);
      token = strtok(get_buffer, ",;");data[0] = atoi(token); //get Speed
      token = strtok(NULL, ",;");data[1] = atoi(token); //get Roll
      sendable = true;
    }
    
}

int main() {
  buffer_T[4] = '\0', buffer_H[4] = '\0';
  Servo.period(0.02f);
  Propeller.period(0.02f);
  float degree = 0;
  int PWM = 0;
  Timer T;
  T.start();
  PC.printf("Slave\n");
  while(1) {
    if(bluetooth.readable()){
      BluetoothReceived();
      sendable = true;
      degree = float(data[1]) * 180 / 1024;
    }
    // if(currentDegree < degree){//Servo
    //   currentDegree += 1;
    //   setServo(currentDegree);
    // }
    // else if(currentDegree > degree){
    //   currentDegree -= 1;
    //   setServo(currentDegree);
    // }
    

    if((sendable && T.read() > 0.02)){
      T.stop();T.reset();T.start();
      dht22.readData();
      data[2] = int(dht22.ReadTemperature());
      data[3] = int(dht22.ReadHumidity());
      sprintf(buffer_T, "%04d", data[2]);//Temp
      sprintf(buffer_H, "%04d", data[3]);//Humid
      PC.printf("Temp = [%s]\n", buffer_T);
      PC.printf("Humid = [%s]\n", buffer_H);
      sprintf(send_buffer, "%s,%s;", buffer_T, buffer_H);
      PC.printf("send_buffer = [%s]\n", send_buffer);
      bluetooth.puts(send_buffer);
      sendable = false;
    }

    if(abs(currentDegree - degree) > 5){//update servo degree
      currentDegree = degree;
      setServo(currentDegree);
      }
    if(abs(currentSpeed - data[0]) > 5){
      currentSpeed = data[0];//data[0] มีค่าตั่งเเต่ 0 - 1023
      setPropeller(float(currentSpeed)/1024/2);//PWM //limited at 50%
    }
    
  }
}