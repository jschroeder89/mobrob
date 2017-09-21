#ifndef requestHandler_HPP
#define requestHandler_HPP

//Includes
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <ArduinoJson.h>
#include "/home/johann/github/mobrob/CC/lib/dataHandler/dataHandler.hpp"

//Defines
#define sensorRead 1
#define sensorReadByte '1'
#define servoRead 2
#define servoReadByte '2'
#define servoWrite 3
#define servoWriteByte '3'
#define bufLen 1024
#define jsonBufLen 1024

//Prototypes
int openPort(char const *port);
int readFromUSB(int fd);
void requestHandler(int fd, int op);
void convertVelocitiesToJson(int fd, int velLeft, int velRight);
void writeToUSB(int fd, JsonObject& root);
void setVelocities(int fd, int velLeft, int velRight);
void jsonSensorParser(std::string json);
void jsonServoParser(std::string json);

#endif
