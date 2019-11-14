#include "mbed.h"
#include "TextLCD.h"
#include "DHT22.h"
 
// Host PC Communication channels
Serial PC(USBTX, USBRX), bluetooth(D8, D2);//Bluetooth (RX, TX)
 
// I2C Communication
I2C i2c_lcd(D14,D15); // LCD (SDA, SCL) ***Pull UP Resistor!!!!
 
// LCD instantiation 
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);// I2C exp: I2C bus, PCF8574 Slaveaddress, LCD Type
int columns = lcd.columns(), rows = lcd.rows();
bool display_mode = true, button_state, first_round = true, sendable=true;
int buff,i=0;
char buffer[9];
char buffer1[5];
char display_buffer[6];
uint16_t data[4];
AnalogIn X(A0), Y(A1);
DigitalIn A(D3), B(D4);
int degree, PWM;
int highByte, lowByte;

DHT22 dht22(D4); // Notice DHT22 named dht22

void split(int n) {
    highByte = (n / 128) + 1;
    lowByte = n % 128 + 1;
}

void BluetoothReceived(void){
    bluetooth.gets(buffer, sizeof(buffer));
    for(int j=0; j < 4; j++){
      data[j] = (buffer[j*2]-1)*128 + (buffer[j*2+1]-1);
      // PC.printf("data[%d] = %d\n", j, data[j]);
      // PC.printf("data[%d] = %d, %d\n", j, buffer[j*2]-1, buffer[j*2+1]-1);
    }
}

int main() {
  PC.baud(38400);
  bluetooth.baud(38400);
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
      sprintf(display_buffer, "%d", data[0]);
      lcd.locate(6,0);lcd.puts(display_buffer);//Update Speed
      sprintf(display_buffer, "%d", data[1]);
      lcd.locate(6,1);lcd.puts(display_buffer);//Update Roll
      }
      else{
        sprintf(display_buffer, "%d", data[2]);
        lcd.locate(6,0);lcd.puts(display_buffer);//Update Temp
        sprintf(display_buffer, "%d", data[3]);
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
	  
	  dht22.readData(); // Read data
	  data[3] = int(dht22.ReadTemperature()); // Read temperature
	  data[4] = int(dht22.ReadHumidity()); // Read Himidity
	  
      degree = int(X.read() * 1023);
      PWM = int(Y.read() * 1023);
      // PC.printf("%d, %d\n", degree, PWM);
      split(degree);
      buffer1[0] = highByte;buffer1[1] = lowByte;
      split(PWM);
      buffer1[2] = highByte;buffer1[3] = lowByte;
      bluetooth.puts(buffer1);
      sendable = false;
    }
  }
}