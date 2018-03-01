#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include "requestHandler.hpp"
#include <chrono>
#include <thread>


/*############  Non-Class Functions  ############*/
int openPort(char const *port) {
    int fd = 0;
    struct termios newtio, oldtio;
    fd = open(port, O_RDWR);
    if  (fd == -1) {
        perror("Port not found");
        return -1;
    }

    //tcgetattr(fd, &oldtio);
    //newtio.c_cflag = 9600 | CRTSCTS | CLOCAL;
    //newtio.c_iflag = IGNPAR;
    //newtio.c_oflag = 0;
    //newtio.c_lflag = 0;
    //newtio.c_cc[VTIME] = 0;
    //newtio.c_cc[VMIN] = 30;
    //tcflush(fd, TCIFLUSH);
    //tcsetattr(fd,TCSANOW,&newtio);
    return fd;
}

int bar(int fd) {
    char buf[30];
    int numread, nbytes = 0;
    int returnval;

    while (numread < nbytes - 1) {
        returnval = read(fd, buf + numread, numread);
        std::cout << returnval << '\n';
        if ((returnval == -1) && (errno == EINTR))
            continue;
        if ( (returnval == 0) && (numread == 0) )
            return 0;
        if (returnval == 0)
            break;
        if (returnval == -1)
            return -1;
        numread++;
        if (buf[numread-1] == '\n') {
            buf[numread] = '\0';
            return numread;
        }
    }
    errno = EINVAL;
    return -1;
}

std::string serialRead(int fd) {
    char buf[jsonBufLenLong];
    int n = 0, nbytes = 0;
    std::string json;

    n = read(fd, buf, jsonBufLenLong);
    buf[jsonBufLenLong-1] = '\0';
    json = buf;
    if (n >= 0) {
        std::cout << "No Bytes read!" << '\n';
        return "";
    }
    size_t jsonStartPos = json.find('{');
    json = json.erase(0, jsonStartPos);
    size_t jsonEndPos = json.find('}');
    json = json.erase(jsonEndPos+1, json.length());

    return json;
}

std::string readFromUSB(int fd) {
    char buf[jsonBufLenLong];
    int n = 0, nbytes = 0;
    std::string json;

    //do {
    
        n = read(fd, buf, jsonBufLenLong);
        //buf[nbytes+1] = '\0';
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        buf[jsonBufLenLong-1] = '\0';
        json = buf;
        /*if (buf[n] == '}') {
            buf[nbytes] = '\0';
            json = buf;
            std::cout << json << '\n';
            break;
        }
        nbytes = n + nbytes;
    } while(nbytes <= 100);
    */
    size_t jsonStartPos = json.find('{');
    json = json.erase(0, jsonStartPos);
    size_t jsonEndPos = json.find('}');
    json = json.erase(jsonEndPos+1, json.length());
    return json;
}

int writeToUSB(int fd, JsonObject& root) {
    std::string json;
    root.printTo(json);
    int n = write(fd, &json, json.size());
    if (n < 0) {
        perror("Unable to write...");
        return -1;
    }
    if (n == 0) {
        perror("No bytes written...");
        return 0;
    }
    return n;
}

void requestHandler(int fd, int op) {
    char byte = '0';

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
    int n = write(fd, &byte, 1);
    //std::cout << byte << '\n';
    if (n < 0) {
        std::cout << "No bytes writen!" << std::endl;
    }
}


std::string guiDataToJsonString(std::vector<int>& sensorData, std::vector<float>& coords) {
    std::string json;
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["message"] = "zmq";
    JsonArray& sensor = root.createNestedArray("sensor");
    JsonArray& path = root.createNestedArray("path");

    for (size_t i = 0; i < sensorData.size(); i++) {
        sensor.add(sensorData.at(i));
    }
    for (size_t i = 0; i < coords.size(); i++) {
        path.add(coords.at(i));
    }
    root.printTo(json);
    return json;
}

/*############  Sensor Class Methods  ############*/
Sensor::Sensor() {
    std::vector<int> temp;
    for (size_t i = 0; i < 5; i++) {
        sensorData.push_back(temp);
    }
}

Sensor::~Sensor() {
    sensorData.clear();
}

std::string Sensor::requestSensorDataJsonString(int fd) {
    requestHandler(fd, sensorRead);
    std::string json = readFromUSB(fd);
    return json;
}

void Sensor::jsonToSensorData(std::string json) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    for (size_t i = 0; i < 8; i++) {
        sensorData.at(0).push_back(root["FL"][i]);
        sensorData.at(1).push_back(root["FR"][i]);
        sensorData.at(2).push_back(root["L"][i]);
        sensorData.at(3).push_back(root["B"][i]);
        sensorData.at(4).push_back(root["R"][i]);
    }
}

Sensor::hasContact Sensor::detectCollisionSide() {
    Sensor::hasContact collisionSide = Sensor::hasContact::none;
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 8; j++) {
            if (sensorData.at(i).at(j) > collisionThreshold) {
                switch (i) {
                    case 0:
                        collisionSide = Sensor::hasContact::frontLeft;
                        break;
                    case 1:
                        collisionSide = Sensor::hasContact::frontRight;
                        break;
                    case 2:
                        collisionSide = Sensor::hasContact::left;
                        break;
                    case 3:
                        collisionSide = Sensor::hasContact::rear;
                        break;
                    case 4:
                        collisionSide = Sensor::hasContact::right;
                        break;
                }
            }
        }
    }
    return collisionSide;
}

std::vector<int> Sensor::flatData() {
    std::vector<int> flatData;
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 8; j++) {
            flatData.push_back(sensorData[i][j]);
            std::cout << sensorData[i][j] << '\n';
        }
    }
    return flatData;
}

/*############  Servo Class Methods  ############*/
Servo::Servo() {

}

Servo::~Servo() {
    velocities.clear();
    velocitiesMeterPerSec.clear();
}

JsonObject& Servo::velocitiesToJson(std::vector<int>& velocities) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["data"] = "velocities";
    root["velLeft"] = velocities.at(0);
    root["velRight"] = velocities.at(1);
    return root;
}

std::string Servo::requestServoDataJsonString(int fd) {
    requestHandler(fd, servoRead);
    std::string json = readFromUSB(fd);
    std::cout << json << '\n';
    return json;
}

void Servo::jsonToServoData(std::string json) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    velocities.push_back(root["velLeft"]);
    velocities.push_back(root["velRight"]);
}

void Servo::velocitiesInMeterPerSec() {
    for (int i = 0; i < 2; i++) {
        velocitiesMeterPerSec.push_back(
        wheelDiameter * M_PI * revolutionsPerMinute/60 * velocities.at(i));
    }
}

std::vector<float> Servo::calculateCoords(std::vector<float>& previousCoords, float t) {
    std::vector<float> newCoords;
    previousCoords.reserve(3);
    float distanceLeft = t * velocitiesMeterPerSec.at(0);
    float distanceRight = t * velocitiesMeterPerSec.at(1);
    float distanceMiddle = (distanceLeft + distanceRight) / 2;
    float distanceDif = (distanceRight - distanceLeft) / 2 * axisLength;

    float theta = (distanceRight - distanceLeft)/axisLength + previousCoords.at(0);
    newCoords.push_back(theta);

    float x = distanceMiddle * cosf(theta + distanceDif) + previousCoords.at(1);
    newCoords.push_back(x);

    float y = distanceMiddle * sinf(theta + distanceDif) + previousCoords.at(2);
    newCoords.push_back(y);

    return newCoords;
}
