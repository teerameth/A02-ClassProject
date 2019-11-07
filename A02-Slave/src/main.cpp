#include <mbed.h>

Serial PC(USBTX, USBRX), bluetooth(D5, D4);
DigitalOut led(LED1);
uint8_t receive = 0;
char buffer[8];
int data[4] = {10, 20, 30, 40};
int highByte, lowByte;
void split(int n) {
    highByte = n / 128;
    lowByte = n % 128;
}


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
      
      // bluetooth.putc(a);
      // bluetooth.putc(b);
      // bluetooth.putc(c);
      // bluetooth.putc(d);
      // bluetooth.putc('\n');
      for(int i=0;i<4;i++){
        split(data[i]);
        buffer[i*2] = highByte;
        buffer[i*2+1] = lowByte;
      }
      bluetooth.puts(buffer);
      led = !led;
    }
  }
}