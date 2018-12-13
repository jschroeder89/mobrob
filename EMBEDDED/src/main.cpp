#include <DynamixelController.hpp>

void setup() {
    I2C i2c;
    Serial.begin(115200);
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    initializeUART();
    initializeServos();
    i2c.initializeI2C();
    digitalWrite(13, LOW);
}

void loop() {
    //rxSerialEventUsb();
    requestHandler();
    delay(30);
    //Serial.flush();
}
