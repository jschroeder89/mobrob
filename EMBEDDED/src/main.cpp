#include <DynamixelController.hpp>

int count;
void setup() {
    I2C i2c;
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    initializeUART();
    initializeServos();
    i2c.initializeI2C();
    digitalWrite(13, LOW);
}

void loop() {
    //count++;
    //Serial.println(count);
    //rxSerialEventUsb();
    requestHandler();
    //servoReadPcktConstructor();
    //Serial.println("HELLO WORLD");
    delay(50);
    Serial.flush();
}
