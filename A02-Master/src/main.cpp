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
char get_buffer[11], send_buffer[11];//"0000,0000;" -> 11
char* token;
char buffer_X[5], buffer_Y[5];
int16_t data[4] = {50, 0, 25, 50};//Speed, Roll, Temp, Humid


void BluetoothReceived(void){
    bluetooth.gets(get_buffer, 11);
    PC.printf("get_buffer = [%s]\n", get_buffer);
    if (get_buffer[4] == ',' && get_buffer[9] == ';'){
      token = strtok(get_buffer, ",;");data[2] = atoi(token); //get Temp
      token = strtok(NULL, ",;");data[3] = atoi(token); //get Humid
    }
    sendable = true;
}

int main() {
  lcd.setBacklight(TextLCD_I2C::LightOn);
  lcd.cls();
  Timer T;
  T.start();
  PC.printf("Ready\n");
  buffer_X[4] = '\0';buffer_Y[4] = '\0';
  while (1){
    if(bluetooth.readable()){
      BluetoothReceived();
      sendable = true;
    }

    //Control Plane
    if((sendable && T.read() > 0.02) || T.read()>0.5){
      T.stop();T.reset();T.start();
      sprintf(buffer_Y, "%04d", data[0]);//Speed
      sprintf(buffer_X, "%04d", data[1]);//Roll
      sprintf(send_buffer, "%s,%s;", buffer_Y, buffer_X);
      PC.printf("send_buffer = [%s]\n", send_buffer);
      bluetooth.puts(send_buffer);

      sendable = false;
    }
    if(display_mode){
        sprintf(display_buffer, "%d%%   ", int(float(data[0])/1024*100));
        lcd.locate(6,0);lcd.puts(display_buffer);//Update Speed
        sprintf(display_buffer, "%d    ", (int)(float(data[1]) / 1023 * 180) - 90);
        lcd.locate(6,1);lcd.puts(display_buffer);//Update Roll
      }
    else{
        sprintf(display_buffer, "%d    ", data[2]);
        lcd.locate(6,0);lcd.puts(display_buffer);//Update Temp
        sprintf(display_buffer, "%d    ", data[3]);
        lcd.locate(6,1);lcd.puts(display_buffer);//Update Humid
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
    //Read from joystick and update
    data[0] = int(Y.read() * 1023);//Speed
    data[1] = int(X.read() * 1023);//Roll
    
    
  }
}