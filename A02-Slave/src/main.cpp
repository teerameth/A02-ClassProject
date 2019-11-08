#include <mbed.h>

Serial PC(USBTX, USBRX), bluetooth(D5, D4);
DigitalOut led(LED1);
uint8_t receive = 0;
char buffer[9];
uint16_t data[4] = {10, 20, 30, 40};
int highByte, lowByte;

void split(int n) {
    highByte = (n / 128)%128 + 1;
    lowByte = n % 128 + 1;
}


int main() {
  PC.baud(38400);
  bluetooth.baud(38400);
  Timer T;
  T.start();
  PC.printf("Slave\n");
  while(1) {
    if(T.read() > 0.2){
      T.stop();
      T.reset();
      T.start();
      for(int i=0;i<4;i++){
        split(data[i]);
        buffer[i*2] = highByte;
        buffer[i*2+1] = lowByte;
      }
      bluetooth.puts(buffer);
      // bluetooth.puts("12345678");
      led = !led;
    }
  }
}