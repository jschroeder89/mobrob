#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <math.h>
//#include <dataHandler.hpp>
#include "requestHandler.hpp"

//Sensor::hasContact Sensor::collisionSide;
std::vector<int> Sensor::sensorDataFlat;
std::vector<float> Servo::velocitiesMeterPerSec;
std::vector<float> Servo::coords;

int openPort(char const *port) {
    int fd = open(port, O_RDWR | O_NDELAY);
        if  (fd == -1) {
            perror("Port not found");
            return -1;
        }
        /*struct termios options;

        tcgetattr(fd, &options);
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_iflag &= ~(BRKINT|ICRNL|ISTRIP|IXON);
        options.c_oflag = 0;
        options.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
        tcflush( fd, TCIFLUSH );
        tcsetattr(fd, TCSANOW, &options);*/
    return fd;
}

void requestHandler(int fd, int op) {
    char byte;

    switch (op) {
        case sensorRead:
        byte = sensorReadByte;
        break;
            case servoRead:
            byte = servoReadByte;
            break;
                case servoWrite:
                byte = servoWriteByte;
                break;
    }
    int n = write(fd, &byte, sizeof byte);
    if (n < 0) {
        std::cout << "No bytes writen!" << std::endl;
    }
}

int readFromUSB(int fd, int op) {
    char buf[bufLen];
    int n = 0, nbytes = 0;
    std::string json;

    do {
        n = read(fd, buf+nbytes, bufLen-nbytes);
        if (buf[nbytes-1] == '}') {
            buf[nbytes] = '\0';
            json = buf;
            break;
        }
        nbytes +=n;
    } while(nbytes <= bufLen);

    size_t jsonStartPos = json.find('{');
    json = json.erase(0, jsonStartPos);
    size_t jsonEndPos = json.find('}');
    json = json.erase(jsonEndPos+1, json.length());

    if (op == sensorRead) {
        jsonSensorParser(json);
    } else if (op == servoRead) {
        jsonServoParser(json);
    }
    return 0;
}

void jsonSensorParser(std::string json) {
    StaticJsonBuffer<jsonBufLen> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    std::vector<int> FL, FR, L, R, B, flatData;
    std::vector<std::vector<int> > sensorData;

    for (size_t i = 0; i < 8; i++) {
        FL.push_back(root["FL"][i]);
        FR.push_back(root["FR"][i]);
        L.push_back(root["L"][i]);
        B.push_back(root["B"][i]);
        R.push_back(root["R"][i]);
    }
    sensorData.push_back(FL);
    sensorData.push_back(FR);
    sensorData.push_back(L);
    sensorData.push_back(B);
    sensorData.push_back(R);

    Sensor::sensorDataFlat.reserve(40);
    Sensor::sensorDataFlat.insert(Sensor::sensorDataFlat.end(), FL.begin(), FL.end());
    Sensor::sensorDataFlat.insert(Sensor::sensorDataFlat.end(), FR.begin(), FR.end());
    Sensor::sensorDataFlat.insert(Sensor::sensorDataFlat.end(), L.begin(), L.end());
    Sensor::sensorDataFlat.insert(Sensor::sensorDataFlat.end(), B.begin(), B.end());
    Sensor::sensorDataFlat.insert(Sensor::sensorDataFlat.end(), R.begin(), R.end());

    detectCollisionSide(sensorData);
}

void detectCollisionSide(std::vector<std::vector<int> >& v) {
    Sensor::collisionSide = Sensor::hasContact::none;

    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 8; j++) {
            if (v.at(i).at(j) > collisionThreshold) {
                switch (i) {
                    case 0:
                        Sensor::collisionSide = Sensor::hasContact::frontLeft;
                        break;
                    case 1:
                        Sensor::collisionSide = Sensor::hasContact::frontRight;
                        break;
                    case 2:
                        Sensor::collisionSide = Sensor::hasContact::left;
                        break;
                    case 3:
                        Sensor::collisionSide = Sensor::hasContact::rear;
                        break;
                    case 4:
                        Sensor::collisionSide = Sensor::hasContact::right;
                        break;
                }
            }
        }
    }
}

void jsonServoParser(std::string json) {
    StaticJsonBuffer<jsonBufLen> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    std::vector<int> servoData;

    servoData.push_back(root["velLeft"]);
    servoData.push_back(root["velRight"]);

    std::cout << servoData[0] << std::endl << servoData[1] << std::endl;
    convertTicksToVelocities(servoData);
}


void convertTicksToVelocities(std::vector<int>& v) {
    for (int i = 0; i < 2; i++) {
        Servo::velocitiesMeterPerSec.push_back(
        wheelDiameter * M_PI * revolutionsPerMinute/60 * v.at(i));
    }
}

void getCoordinates(float t) {
    float distanceLeft = t * Servo::velocitiesMeterPerSec.at(0);
    float distanceRight = t * Servo::velocitiesMeterPerSec.at(1);
    float distanceMiddle = (distanceLeft + distanceRight) / 2;
    float distanceDif = (distanceRight - distanceLeft) / 2 * axisLength;

    float theta = (distanceRight - distanceLeft)/axisLength + Servo::coords.at(2);
    Servo::coords.at(2) = theta;

    float x = distanceMiddle * cosf(Servo::coords.at(2) * distanceDif) + Servo::coords.at(0);
    Servo::coords.at(0) = x;

    float y = distanceMiddle * sinf(Servo::coords.at(2) * distanceDif) + Servo::coords.at(1);
    Servo::coords.at(1) = y;

    Servo::velocitiesMeterPerSec.clear();
}

std::string GUIData() {
    std::string json;
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["message"] = "zmq";
    JsonArray& sensor = root.createNestedArray("sensor");
    JsonArray& path = root.createNestedArray("path");

    for (size_t i = 0; i < Sensor::sensorDataFlat.size(); i++) {
        sensor.add(Sensor::sensorDataFlat.at(i));
    }
        for (size_t i = 0; i < Servo::coords.size(); i++) {
            path.add(Servo::coords.at(i));
        }

    Sensor::sensorDataFlat.clear();
    Servo::coords.clear();
    root.printTo(json);

    return json;
}

void writeToUSB(int fd, JsonObject& root, int bufferSize) {
    char buffer[bufferSize];
    root.printTo(buffer, bufferSize+1);
    int n = write(fd, &buffer, bufferSize+1);
    if (n < 0) {
        std::cout << "Could not write to USB port..." << std::endl;
    }
}

void setVelocities(int fd, int velLeft, int velRight) {
    convertVelocitiesToJson(fd, velLeft, velRight);
}

void convertVelocitiesToJson(int fd, int velLeft, int velRight) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["data"] = "servoVels";
    root["velLeft"] = velLeft;
    root["velRight"] = velRight;

    int bufferSize = root.measureLength();

    writeToUSB(fd, root, bufferSize);
}

void requestSensorData(int fd) {
    requestHandler(fd, sensorRead);
    readFromUSB(fd, sensorRead);
}

void requestServoData(int fd) {
    requestHandler(fd, servoRead);
    readFromUSB(fd, servoRead);
}

void setServoVelocities(int fd, int velLeft, int velRight) {
    requestHandler(fd, servoWrite);
    setVelocities(fd, velLeft, velRight);
}
