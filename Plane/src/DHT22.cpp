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
        Check loop data 40 bits and all data set 0.
    */
    for (i = 0; i < DHT22_DATA_BIT_COUNT; i++) {  
        bitTimes[i] = 0;
    }

    /*
        If _firsttime state is false, that means it's too quick to send data.
		or Check if sensor was read less than two seconds ago and return early to use last reading.
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
    } while ((DHT22_io==0)); // Exit on DHT22 push high within 80us

    if (err != ERROR_NONE) {
        return err;
    }

    // Reading the 5 byte data stream
    // Step 2: DHT22 send data to MCU
    //         Start bit -> low volage within 50us (actually could be anything from 35-75us)
    //         0         -> high volage within 26-28us (actually could be 10-40us)
    //         1         -> high volage within 70us (actually could be 60-85us)

    /*
        Let's create loop reading 41 Bits
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
	/* 
		If read sccessfully data, change mode MCU to OUTPUT and set High-Signal since it's read sccessful.
		Re-init DHT22 pin.
	*/

    DHT22_io.output();
    DHT22_io = 1;
	
	/*
		Keep each 8 bits move to 1 bytes. (From Left to Right)
	*/
	
    for (i = 0; i < 5; i++) {
        b=0;
        for (j=0; j<8; j++) {
			if (bitTimes[i*8+j+1] > 0) {
                b |= ( 1 << (7-j));
            }
        }
        DHT22_data[i]=b;
    }
	
	/*
		Checksum before keep least value.
		- Unless parity bit equals 4 array first (4 byte first), it will keep error checksum value.
		- If parity bit equals 4 array first (4 byte first), it will keep a least current time value, a least temperature value and 
	*/

    if (DHT22_data[4] == ((DHT22_data[0] + DHT22_data[1] + DHT22_data[2] + DHT22_data[3]) & 0xFF)) {
        _lastReadTime = currentTime;
        _lastTemperature=CalcTemperature();
        _lastHumidity=CalcHumidity();

    } else {
        err = ERROR_CHECKSUM;
    }

    return err;

}

/*
	Calculate Temperature
*/

float DHT22::CalcTemperature() {
    float v; // Notice one variables
    v = DHT22_data[2] & 0x7F; // Bound the most significant bit (Bit14 ~ bit 0) temperature values.
    v *= 256; // Since it's high byte, need to multiple 256.
    v += DHT22_data[3]; // Plus low byte.
    v /= 10; // Make 1 significant
    if (DHT22_data[2] & 0x80)   // If the temperature is the highest bit (Bit15) is equal to 1 indicates a negative temperature.
    {
        v *= -1; // Multiple 1.
    }
    return float(v); // Return Float v Value.
}

/*
	Read Humidity
*/

float DHT22::ReadHumidity() {
    return _lastHumidity;
}
/*
	Convert from Celcius to Farenheit
*/
float DHT22::ConvertCelciustoFarenheit(float celsius) {
    return celsius * 9 / 5 + 32;
}
/*
	Convert from Celcius to Kelvin
*/
float DHT22::ConvertCelciustoKelvin(float celsius) {
    return celsius + 273.15;
}
/*
	Read Temperature
*/
float DHT22::ReadTemperature() {
        return _lastTemperature;
}
/*
	Read Temperature Select Units
*/
float DHT22::ReadTemperature(eScale Scale) {
    if (Scale == FARENHEIT)
        return ConvertCelciustoFarenheit(_lastTemperature);
    else if (Scale == KELVIN)
        return ConvertCelciustoKelvin(_lastTemperature);
    else
        return _lastTemperature;
}

float DHT22::CalcHumidity() {
    float v; // Notice one variables
    v = DHT22_data[0]; // Bound the most significant bit (Bit15 ~ bit 0) temperature values.
    v *= 256; // Since it's high byte, need to multiple 256.
    v += DHT22_data[1]; // Plus low byte.
    v /= 10; // Make 1 significant
    return float(v); // Return Float v Value.
}