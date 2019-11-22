#include "mbed.h"
#include "TextLCD.h"
 
// Host PC Communication channels
Serial PC(USBTX, USBRX, 38400), bluetooth(D8, D2, 38400);//Bluetooth (RX, TX)
 
// I2C Communication
I2C i2c_lcd(D14,D15); // LCD (SDA, SCL) ***Pull UP Resistor!!!!
 
// LCD instantiation 
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);// I2C exp: I2C bus, PCF8574 Slaveaddress, LCD Type
int columns = lcd.columns(), rows = lcd.rows();
bool display_mode = true, button_state, first_round = true, sendable=true;
char display_buffer[6];

AnalogIn X(A0), Y(A1);
DigitalIn A(D3), B(D4);
int degree, PWM;
// int highByte, lowByte;
char get_buffer[21], send_buffer[11];//"0000,0000,0000,0000;" -> 21   "0000,0000;" -> 11
char* token;
char buffer_X[4], buffer_Y[4];
int16_t data[4] = {50, 0, 25, 50};//Speed, Roll, Temp, Humid


void BluetoothReceived(void){
    bluetooth.gets(get_buffer, 17);
    PC.printf("%s\n", get_buffer);
    token = strtok(get_buffer, ",;");data[0] = atoi(token);
    token = strtok(NULL, ",;");data[1] = atoi(token);
    token = strtok(NULL, ",;");data[2] = atoi(token);
    token = strtok(NULL, ",;");data[3] = atoi(token);
    sendable = true;
}

int pow(int m, int n){
  int a = 1;
  for(int j=0;j<n;j++){
    a *= 10;
  }
  return a;
}

int main() {
  lcd.setBacklight(TextLCD_I2C::LightOn);
  lcd.cls();
  Timer T;
  T.start();
  PC.printf("Ready\n");
  while (1){
    if(bluetooth.readable()){
      BluetoothReceived();
      if(display_mode){
        sprintf(display_buffer, "%d%%   ", int(float(data[0])/1024*100));
        lcd.locate(6,0);lcd.puts(display_buffer);//Update Speed
        sprintf(display_buffer, "%d    ", data[1]-90);
        lcd.locate(6,1);lcd.puts(display_buffer);//Update Roll
      }
      else{
        sprintf(display_buffer, "%d    ", data[2]);
        lcd.locate(6,0);lcd.puts(display_buffer);//Update Temp
        sprintf(display_buffer, "%d    ", data[3]);
        lcd.locate(6,1);lcd.puts(display_buffer);//Update Humid
       }
    }
    if(!A && button_state || !B && button_state){button_state = false;}
    if(A == 1 && !button_state || first_round){
      first_round = false;
      button_state = true;
      if(A==1){display_mode = true;}
      if(B==1){display_mode = false;}
      if(display_mode){
        lcd.locate(0,0);lcd.puts("Speed:");
        lcd.locate(0,1);lcd.puts("Roll :");
      }
      else{
        lcd.locate(0,0);lcd.puts("Temp :");
        lcd.locate(0,1);lcd.puts("Humid:");
      }
    }
    if((sendable && T.read() > 0.2) || T.read()>0.5){
      T.stop();T.reset();T.start();
      degree = int(X.read() * 1023);
      PWM = int(Y.read() * 1023);
      sprintf(buffer_X, "%04d", degree);
      sprintf(buffer_Y, "%04d", PWM);
      sprintf(send_buffer, "%s,%s;", buffer_X, buffer_Y);
      bluetooth.puts(send_buffer);
      sendable = false;
    }
  }
}