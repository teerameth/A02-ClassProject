#include "mbed.h"
#include "TextLCD.h"
 
// Host PC Communication channels
Serial PC(USBTX, USBRX), bluetooth(D8, D2);//Bluetooth (RX, TX)
 
// I2C Communication
I2C i2c_lcd(D14,D15); // LCD (SDA, SCL) ***Pull UP Resistor!!!!
 
// LCD instantiation 
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);// I2C exp: I2C bus, PCF8574 Slaveaddress, LCD Type
int columns = lcd.columns(), rows = lcd.rows();
bool display_mode = true, button_state;
int buff,i=0;
char buffer[9];
char* display_buffer;
uint8_t data[4];
DigitalIn mode_button(USER_BUTTON);

void onBluetoothReceived(void){
  if(bluetooth.readable()){
    bluetooth.gets(buffer, sizeof(buffer));
    for(int j=0; j < sizeof(data); j++){
      data[j] = (buffer[j*2]-1)*128 + (buffer[j*2+1]-1);
      PC.printf("data[%d] = %d\n", j, data[j]);
    }
  }
}

int main() {
  PC.baud(38400);
  bluetooth.baud(38400);
  lcd.setBacklight(TextLCD_I2C::LightOn);
  lcd.cls();
  bluetooth.attach(&onBluetoothReceived, Serial::RxIrq);
  PC.printf("Ready\n");
  while (1)
  {
    if(!mode_button && button_state){button_state = false;}
    if(mode_button == 1 && !button_state){
      button_state = true;
      display_mode = !display_mode;
      if(display_mode){
        lcd.locate(0,0);lcd.puts("Speed:");
        lcd.locate(0,1);lcd.puts("Roll :");
      }
      else{
        lcd.locate(0,0);lcd.puts("Temp :");
        lcd.locate(0,1);lcd.puts("Humid:");
      }
    }
    if(display_mode){
      sprintf(display_buffer, "%.2f", data[0]);
      lcd.locate(6,0);lcd.puts(display_buffer);//Update Speed
      sprintf(display_buffer, "%.2f", data[1]);
      lcd.locate(6,1);lcd.puts(display_buffer);//Update Roll
    }
    else{
      sprintf(display_buffer, "%.2f", data[2]);
      lcd.locate(6,0);lcd.puts(display_buffer);//Update Temp
      sprintf(display_buffer, "%.2f", data[3]);
      lcd.locate(6,1);lcd.puts(display_buffer);//Update Humid
     }
  }
}