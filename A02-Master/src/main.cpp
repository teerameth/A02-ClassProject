#include "mbed.h"
#include "TextLCD.h"
 
// Host PC Communication channels
Serial PC(USBTX, USBRX), bluetooth(D8, D2);
 
// I2C Communication
I2C i2c_lcd(D14,D15); // SDA, SCL
 
// LCD instantiation 
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);// I2C exp: I2C bus, PCF8574 Slaveaddress, LCD Type
int columns = lcd.columns(), rows = lcd.rows();
bool display_mode = true, button_state;
char* buffer;
DigitalIn mode_button(USER_BUTTON);

int main() {
  PC.baud(38400);
  bluetooth.baud(38400);
  lcd.setBacklight(TextLCD_I2C::LightOn);
  lcd.cls();
  PC.printf("Ready\n");
  while (1)
  {
    if (bluetooth.readable())
    {
      PC.putc(bluetooth.getc());
    }
    else{
      // bluetooth.printf("A");
    }
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
      lcd.locate(6,0);lcd.puts("");//Update Speed
      lcd.locate(6,1);lcd.puts("");//Update Roll
    }
    else{
      lcd.locate(6,0);lcd.puts("");//Update Temp
      lcd.locate(6,1);lcd.puts("");//Update Humid
    }
  }
}