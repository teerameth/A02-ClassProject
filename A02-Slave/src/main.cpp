#include <mbed.h>
Serial PC(USBTX, USBRX), bluetooth(D5, D4);
DigitalOut led(LED1);
uint8_t receive = 0;

int a=10, b=20, c=30, d=40;
int main() {
  PC.baud(38400);
  bluetooth.baud(38400);
  Timer T;
  T.start();
  while(1) {
    if(T.read() > 0.2){
      T.stop();
      T.reset();
      T.start();
      
      bluetooth.putc(a);
      bluetooth.putc(b);
      bluetooth.putc(c);
      bluetooth.putc(d);
      bluetooth.putc('\n');
      led = !led;
    }
  }
}