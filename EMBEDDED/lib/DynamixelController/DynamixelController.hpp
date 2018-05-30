#ifndef DynamixelController_HPP
#define DynamixelController_HPP

#include <UartEvent.h>
#include <DynamixelMessage.h>
#include <QueueArray.h>
#include <Arduino.h>
#include <string>
#include <vector>
#include <cstdint>
#include <ArduinoJson.h>
#include <mobrobSensor.hpp>

#define sensorReadByte '1'
#define servoReadByte '2'
#define servoWriteByte '3'
#define FF 255
#define _WRITE_LENGHT 5
#define _READ_LENGHT 4
#define _NUM_OF_BYTES_TO_READ 2
#define NumOfSensors 5
#define lenPos 3
#define lenConst 2
#define sensorRead 1
#define servoRead 2
#define servoWrite 3
#define bufLen 1024
#define jsonBufLen 256

void rxEvent1();              //interrupt functions for the input buffer on the UART ports
void rxEvent2();
void rxEvent3();

void txEvent1();                            //interrupt functions for the output buffer on the UART ports
void txEvent2();
void txEvent3();

void rxResync1();
void rxResync2();
void rxResync3();

void noMessageReceival1();                //timer-interrupt functions for timed out messages
void noMessageReceival2();
void noMessageReceival3();

void rxSerialEventUsb();                   //function that listens for bytes on the Serial (USB) port.

void send1();
void send2();
void send3();

void pushToQueue1(DynamixelMessage* messageToPush);
void pushToQueue2(DynamixelMessage* messageToPush);
void pushToQueue3(DynamixelMessage* messageToPush);
void initializeUART();

void initializeServos();

void writeToUSB(JsonArray& velArray, JsonArray& arrayLen);
void convertServoDataToJson(int* dataArray);
void parseJsonString(String s);
void servoWritePcktConstructor(Vector<int>& velArray);
void servoReadPcktConstructor();
void writeToUART(uint8_t* servoPckt);
void convertToReadableVelocities(uint8_t* servoPckt);
void readStatusPckt(uint8_t* statusPckt);
void readFromUSB();

void requestHandler();
void scanPort();
void copyArray(volatile uint8_t* array);

#endif
