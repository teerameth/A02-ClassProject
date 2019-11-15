#include "DHT22.h"              // Include Library DHT-22.

#define DHT22_DATA_BIT_COUNT 41 // This should be 40, but the sensor is adding an extra bit at the start.

DHT22::DHT22(PinName pin) {
    _pin = pin;                 // Set Data Pin.
    _firsttime=true;            // Set _firsttime is true
}

DHT22::~DHT22() {
}

/* 
    Main Function
    To check error communcation or not.
*/

int DHT22::readData() {
    int i, j, retryCount,b;     // Notice 4 Variables
    unsigned int bitTimes[DHT22_DATA_BIT_COUNT];    // Notice 40 Channel Array or 5 Bytes for storing any bits.

    eError err = ERROR_NONE;    // Notice eError named 'err' and default value is ERR0R_NONE. (There's not error.)
    time_t currentTime = time(NULL);                // Notice Real-time named 'currentTime'.

    DigitalInOut DHT22_io(_pin);                    // Notice I/O pin.

    /*
        Check loop data 41 bits and all data set 0.
    */
    for (i = 0; i < DHT22_DATA_BIT_COUNT; i++) {  
        bitTimes[i] = 0;
    }

    /*
        If _firsttime state is false, that means it's too quick to send data.
    */
    if (!_firsttime) {
        if (int(currentTime - _lastReadTime) < 2) {
            err = ERROR_NO_PATIENCE;
            return err;
        }
    } else {
        _firsttime=false;
        _lastReadTime=currentTime;
    }


    retryCount = 0;             // Set retryCount 0

    /*
        Pin needs to start HIGH, wait until it is HIGH with a timeout.
        - if time more than 125*2us = 250 us, it will be bus busy.
        - if it respones, exit on DHT22 retrun 'High' Signal within 250us.
    */
    do {
        if (retryCount > 125) {
            err = BUS_BUSY;
            return err;
        }
        retryCount ++;
        wait_us(2);
    } while ((DHT22_io==0)); // exit on DHT22 retrun 'High' Signal within 250us


    // Send the activate pulse
    // Step 1: MCU send out start signal to DHT22 and DHT22 send response signal to MCU.
    // If always signal high-voltage-level, it means DHT22 is not working properly, plesee check the electrical connection status.
    //

    DHT22_io.output();  // Change I/O to Output mode.
    DHT22_io = 0;       // MCU set low signal.
    wait_ms(18);        // Wait 18 milliseconds.
    DHT22_io = 1;       // MCU set high signal.
    wait_us(40);        // Wait 40 microseconds
    DHT22_io.input();   // Change I/O to Input mode.

    retryCount = 0;     // Set retryCount 0

    /*
        Wait until DHT22 send response signal to MCU.
        - if time more than 40*1us == 40us, it will not be present.
        - if it respones, exit on DHT22 pull low within 40us.
    */

    do {
        if (retryCount > 40)  { // 40*1us == 40us
            err = ERROR_NOT_PRESENT;
            return err;
        }
        retryCount++;
        wait_us(1);
    } while ((DHT22_io==1)); // Exit on DHT22 pull low within 40us

    if (err != ERROR_NONE) {
        return err;
    }

    wait_us(80);    // Wait 80 microseconds
    
    retryCount = 0;     // Set retryCount 0

    do {
        if (retryCount > 80)  { // 80*1us == 80us
            err = ERROR_ACK_TOO_LONG;
            return err;
        }
        retryCount++;
        wait_us(1);
    } while ((DHT22_io==0)); // Exit on DHT22 pull low within 40us

    if (err != ERROR_NONE) {
        return err;
    }

    // Reading the 40 bit data stream
    // Step 2: DHT22 send data to MCU
    //         Start bit -> low volage within 50us
    //         0         -> high volage within 26-28 us
    //         1         -> high volage within 70us
    //

    /*
        Let's create Loop 8 Value of Loop 5 Value (8 Bits of 5 Bytes)

    */ 

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {

            retryCount = 0;
            do {
                if (retryCount > 75)  {
                    err = ERROR_DATA_TIMEOUT;
                    return err;
                }
                retryCount++;
                wait_us(1);
            } while (DHT22_io == 0);
            wait_us(40);
            bitTimes[i*8+j]=DHT22_io;

            int count = 0;
            while (DHT22_io == 1 && count < 100) {
                wait_us(1);
                count++;
            }
        }
    }


    DHT22_io.output();
    DHT22_io = 1;
    for (i = 0; i < 5; i++) {
        b=0;
        for (j=0; j<8; j++) {
            if (bitTimes[i*8+j+1] > 0) {
                b |= ( 1 << (7-j));
            }
        }
        DHT22_data[i]=b;
    }

    if (DHT22_data[4] == ((DHT22_data[0] + DHT22_data[1] + DHT22_data[2] + DHT22_data[3]) & 0xFF)) {
        _lastReadTime = currentTime;
        _lastTemperature=CalcTemperature();
        _lastHumidity=CalcHumidity();

    } else {
        err = ERROR_CHECKSUM;
    }

    return err;

}

float DHT22::CalcTemperature() {
    float v;
    v = DHT22_data[2] & 0x7F;
    v *= 256;
    v += DHT22_data[3];
    v /= 10;
    if (DHT22_data[2] & 0x80)       
    {
        v *= -1;
    }
    return float(v);
}

float DHT22::ReadHumidity() {
    return _lastHumidity;
}

float DHT22::ConvertCelciustoFarenheit(float celsius) {
    return celsius * 9 / 5 + 32;
}

float DHT22::ConvertCelciustoKelvin(float celsius) {
    return celsius + 273.15;
}

float DHT22::ReadTemperature() {
        return _lastTemperature;
}
float DHT22::ReadTemperature(eScale Scale) {
    if (Scale == FARENHEIT)
        return ConvertCelciustoFarenheit(_lastTemperature);
    else if (Scale == KELVIN)
        return ConvertCelciustoKelvin(_lastTemperature);
    else
        return _lastTemperature;
}

float DHT22::CalcHumidity() {
    float v;
    v = DHT22_data[0];
    v *= 256;
    v += DHT22_data[1];
    v /= 10;
    return float(v);
}