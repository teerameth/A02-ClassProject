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
int buff,i=0;
char recieve_buffer[9];
char send_buffer[5];
char display_buffer[6];
uint16_t data[4];
AnalogIn X(A0), Y(A1);
DigitalIn A(D3), B(D4);
int degree, PWM;
int highByte, lowByte;

void split(int n) {
    highByte = (n / 128) + 1;
    lowByte = n % 128 + 1;
}

void BluetoothReceived(void){
    bluetooth.gets(recieve_buffer, sizeof(recieve_buffer));
    for(int j=0; j < 4; j++){
      data[j] = (recieve_buffer[j*2]-1)*128 + (recieve_buffer[j*2+1]-1);
    }
}

int main() {
  lcd.setBacklight(TextLCD_I2C::LightOn);
  lcd.cls();
  Timer T;
  T.start();
  PC.printf("Ready\n");
  while (1)
  {
    if(bluetooth.readable()){
      BluetoothReceived();
      sendable = true;
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
      T.stop();
      T.reset();
      T.start();
      degree = int(X.read() * 1023);
      PWM = int(Y.read() * 1023);
      // PC.printf("%d, %d\n", degree, PWM);
      split(degree);
      send_buffer[0] = highByte;send_buffer[1] = lowByte;//set roll
      split(PWM);
      send_buffer[2] = highByte;send_buffer[3] = lowByte;//set speed
      bluetooth.puts(send_buffer);
      sendable = false;
    }
  }
}