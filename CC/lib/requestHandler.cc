#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <math.h>
//#include <dataHandler.hpp>
#include "requestHandler.hpp"

std::vector<int> Sensor::sensorDataFlat;
Sensor::hasContact collisionSide;
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
    //std::cout << "Written " << n << " Byte(s)" << std::endl;
    //std::cout << byte << std::endl;
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
    std::cout << json << std::endl;
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
    std::vector<int> FL, FR, L, R, B, sensorDataFlat;
    std::vector<std::vector<int> > sensorData = {FL, FR, L, R, B};
    //std::cout << root["FL"][0] << std::endl;

    for (size_t i = 0; i < 8; i++) {
        FL.push_back(root["FL"][i]);
            Sensor::sensorDataFlat.push_back(FL.at(i));
        FR.push_back(root["FR"][i]);
            Sensor::sensorDataFlat.push_back(FR.at(i));
        L.push_back(root["L"][i]);
            Sensor::sensorDataFlat.push_back(L.at(i));
        R.push_back(root["R"][i]);
            Sensor::sensorDataFlat.push_back(R.at(i));
        B.push_back(root["B"][i]);
            Sensor::sensorDataFlat.push_back(B.at(i));
    }

    detectCollisionSide(sensorData);
}

void detectCollisionSide(std::vector<std::vector<int> >& v) {
    Sensor::hasContact collisionSide = Sensor::hasContact::none;

    for (int i = 0; i < 8; i++) {
        if (v.at(0).at(i) >= collisionThreshold
        && collisionSide == Sensor::hasContact::none) {
            collisionSide = Sensor::hasContact::frontLeft;
            break;
        } else {
            collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(1).at(i) >= collisionThreshold
        && collisionSide == Sensor::hasContact::none) {
            collisionSide = Sensor::hasContact::frontRight;
            break;
        } else {
            collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(2).at(i) >= collisionThreshold
        && collisionSide == Sensor::hasContact::none) {
            collisionSide = Sensor::hasContact::right;
            break;
        } else {
            collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(3).at(i) >= collisionThreshold
        && collisionSide == Sensor::hasContact::none) {
            collisionSide = Sensor::hasContact::rear;
            break;
        } else {
            collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(4).at(i) >= collisionThreshold
        && collisionSide == Sensor::hasContact::none) {
            collisionSide = Sensor::hasContact::left;
            break;
        } else {
            collisionSide = Sensor::hasContact::none;
        }
    }
}

void jsonServoParser(std::string json) {
    StaticJsonBuffer<jsonBufLen> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    std::vector<int> servoData;

    servoData.push_back(root["velLeft"]);
    servoData.push_back(root["velRight"]);

    convertTicksToVelocities(servoData);
    std::cout << servoData[0] << std::endl << servoData[1] << std::endl;
}


void convertTicksToVelocities(std::vector<int>& v) {
    for (int i = 0; i < 2; i++) {
        Servo::velocitiesMeterPerSec.at(i) =
        wheelDiameter * M_PI * revolutionsPerMinute/60 * v.at(i);
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

}

char GUIData() {
    int bufferSize = 0;
    char buffer[bufferSize];
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["zmq"] = "message";
    JsonArray& sensor = root.createNestedArray("sensor");
    JsonArray& path = root.createNestedArray("path");

    for (size_t i = 0; i < Sensor::sensorDataFlat.size(); i++) {
        sensor.add(Sensor::sensorDataFlat.at(i));
    }
        for (size_t i = 0; i < Servo::coords.size(); i++) {
            path.add(Servo::coords.at(i));
        }

    bufferSize = root.measureLength();
    root.printTo(buffer, bufferSize+1);

    return *buffer;
}

void writeToUSB(int fd, JsonObject& root, int bufferSize) {
    char buffer[bufferSize+1];
    root.printTo(buffer, sizeof buffer);
    int n = write(fd, &buffer, sizeof buffer);
    if (n > 0) {
        std::cout << "Written " << n << " Byte(s)" << std::endl;
        std::cout << buffer << std::endl;
    }
}

void setVelocities(int fd, int velLeft, int velRight) {
    convertVelocitiesToJson(fd, velLeft, velRight);
}

void convertVelocitiesToJson(int fd, int velLeft, int velRight) {
    StaticJsonBuffer<jsonBufLen> jsonBuffer;
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
