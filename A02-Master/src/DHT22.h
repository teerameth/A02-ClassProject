#ifndef MBED_DHT22_H
#define MBED_DHT22_H

#include "mbed.h"

enum eError {
    ERROR_NONE = 0,
    BUS_BUSY =1,
    ERROR_NOT_PRESENT =2 ,
    ERROR_ACK_TOO_LONG =3 ,
    ERROR_SYNC_TIMEOUT = 4,
    ERROR_DATA_TIMEOUT =5 ,
    ERROR_CHECKSUM = 6,
    ERROR_NO_PATIENCE =7
} ;

typedef enum {
    CELCIUS =0 ,
    FARENHEIT =1,
    KELVIN=2
} eScale;


class DHT22 {

public:

    DHT22(PinName pin);
    ~DHT22();
    int readData(void);
    float ReadHumidity(void);
    float ReadTemperature();
    float ReadTemperature(eScale Scale);

private:
    time_t  _lastReadTime;
    float _lastTemperature;
    float _lastHumidity;
    PinName _pin;
    bool _firsttime;
    int DHT22_data[6];
    float CalcTemperature();
    float CalcHumidity();
    float ConvertCelciustoFarenheit(float);
    float ConvertCelciustoKelvin(float);

};

#endif