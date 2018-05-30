#include "mobrobSensor.hpp"

void I2C::initializeI2C() {
    //Serial.println(F_BUS/20);
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
    //Serial.println("I2C Initialization complete!");
}

void readSensorData() {
    I2C i2c;
    int sensorData[5][8] = {0};

    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 8; j++) {
            sensorData[i][j] = i2c.getI2CSensorData(i, j);
            }
    }
    convertSensorDataToJson(sensorData);
}

int I2C::getI2CSensorData(int add, int ch) {
    Wire.beginTransmission(address[add]);     // slave addr
    Wire.write(channel[ch]);
    int status = Wire.endTransmission();
    if (status == 0)
    {
    Wire.requestFrom(address[add], 2, I2C_STOP);
        while (Wire.available()) {
          int byte1 = Wire.readByte();
          int byte2 = Wire.readByte();
          int number = byte2 | byte1 << 8;
          number /= 16;
          return number;
        }
    }
    return -1;
}

void convertSensorDataToJson(int sensorData[][8]) {
    DynamicJsonBuffer jsonBuffer, lenBuffer;
    JsonArray& sensorArray = jsonBuffer.createArray();
    JsonArray& arrayLen = lenBuffer.createArray();
    for (size_t j = 0; j < 5; j++) {
        for (size_t i = 0; i < 8; i++) {
            sensorArray.add(sensorData[j][i]);
        }
    }
    size_t bufferLen = sensorArray.measureLength();
    arrayLen.add(bufferLen);
    writeSensorDataToUSB(sensorArray, arrayLen);
}

void writeSensorDataToUSB(JsonArray& sensorArray, JsonArray& arrayLen) {
    arrayLen.printTo(Serial);
    sensorArray.printTo(Serial);
    Serial.print('\n');
}
