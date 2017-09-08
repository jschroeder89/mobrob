#ifndef requestHandler_HPP
#define requestHandler_HPP

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <ArduinoJson.h>

//Defines
#define sensorRead 1
#define sensorReadByte '1'
#define servoRead 2
#define servoReadByte '2'
#define servoWrite 3
#define servoWriteByte '3'
#define bufLen 256
#define jsonBufLen 256

//Prototypes
int openPort(char const *port);
int readFromUSB(int fd);
void requestHandler(int fd, int op);
void convertVelocitiesToJson(int fd, int velLeft, int velRight);
void writeToUSB(int fd, JsonObject& root);
void setVelocities(int fd, int velLeft, int velRight);
void jsonSensorParser(char* json);
void jsonServoParser(char* json);

#endif