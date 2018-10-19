
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <math.h>
#include "requestHandler.hpp"

//############  Non-Class Function Prototypes  ############
void publishData(zmq::socket_t& pub, std::string guiData);
int getArrayLen(int fd);
int writeToSerial(int fd, JsonObject& root);
std::string serialRead(int fd);

//Defines
#define jsonBufLenLong 1048
#define jsonBufLenShort 128
#define bufLen 256
#define collisionThreshold 2800
#define axisLength 0.15
#define revolutionsPerMinute 0.916
#define wheelDiameter 0.052

int openPort(char const *port) {
    //int fd = 0;
    struct termios newtio, oldtio;
    int fd = open(port, O_RDWR);
    if  (fd == -1) {
        perror("Port not found");
        return -1;
    }

    tcgetattr(fd, &oldtio);
    newtio.c_cflag = 9600 | CRTSCTS | CLOCAL;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 1;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    return fd;
}

int getArrayLen(int fd) {
    int nbytes = 0, n = 0, bufferLen = 8;
    char buf[bufferLen]; 
    std::string json;
    while(nbytes <= bufferLen) {
        n = read(fd, buf+nbytes, 1);
        /* std::cout << buf << std::endl;
        std::cout << "HERE" << std::endl; */
        if (buf[nbytes] == ']') {
            buf[nbytes+1] = '\0';
            json = buf;
            break;
        }
        nbytes++;
        
    }
    DynamicJsonBuffer jsonBuffer;
    JsonArray& array = jsonBuffer.parse(json);
    int arrayLen = array[0];
    return arrayLen;
}

std::string serialRead(int fd) {
    int n = 0, nbytes = 0, bufferLen = getArrayLen(fd);
    char buf[bufferLen];
    std::string json;
    do {
        n = read(fd, buf+nbytes, bufferLen-nbytes);
        nbytes = n + nbytes;
        if (buf[nbytes-1] == ']') {
            buf[nbytes] = '\0';
            json = buf;
            break;
        }
    } while(nbytes <= bufferLen);
    /* std::cout << json.length() << std::endl;
    std::cout << bufferLen << std::endl; */
    return json;
}

int writeToSerial(int fd, JsonObject& root) {
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
    char requestByte = '0';   
    switch (op) {
        case sensorRead:
            requestByte = sensorReadByte;
            break;
        case servoRead:
            requestByte = servoReadByte;
            break;
        case servoWrite:
            requestByte = servoWriteByte;
            break;
    }
    int n = write(fd, &requestByte, 1);
    if (n <= 0) {
        std::cout << "No bytes writen!" << std::endl;
        std::cout << n << std::endl;
    }
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
    std::string json = serialRead(fd);
    return json;
}

std::vector<int> Sensor::jsonToSensorData(std::string json) {
    int i = 0;
    DynamicJsonBuffer jsonBuffer;
    std::vector<int> flatVec;
    JsonArray& jsonSensorArray = jsonBuffer.parseArray(json);
	for	(i=0; i<8; i++) {
		sensorData.at(0).push_back(jsonSensorArray[i]);
		flatVec.push_back(jsonSensorArray[i]);
	}
	for	(i=8; i<16; i++) {
			sensorData.at(1).push_back(jsonSensorArray[i]);
			flatVec.push_back(jsonSensorArray[i]);
	}
	for	(i=16; i<24; i++) {
			sensorData.at(2).push_back(jsonSensorArray[i]);
			flatVec.push_back(jsonSensorArray[i]);
	}
	for	(i=24; i<32; i++) {
			sensorData.at(3).push_back(jsonSensorArray[i]);
			flatVec.push_back(jsonSensorArray[i]);
	}
	for	(i=32; i<40; i++) {
			sensorData.at(4).push_back(jsonSensorArray[i]);
			flatVec.push_back(jsonSensorArray[i]);
	}
    return flatVec;
}

Sensor::hasContact Sensor::detectCollisionSide() {
    Sensor::hasContact collisionSide = Sensor::hasContact::none;
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 8; j++) {
            if (sensorData.at(i).at(j) > collisionThreshold) {
                switch (i) {
                    case 0:
                        collisionSide = Sensor::hasContact::frontLeft;
                        //std::cout << "FL" << '\n';
                        break;
                    case 1:
                        collisionSide = Sensor::hasContact::frontRight;
                        //std::cout << "FR" << '\n';
                        break;
                    case 2:
                        collisionSide = Sensor::hasContact::left;
                        //std::cout << "L" << '\n';
                        break;
                    case 3:
                        collisionSide = Sensor::hasContact::rear;
                        //std::cout << "B" << '\n';
                        break;
                    case 4:
                        collisionSide = Sensor::hasContact::right;
                        //std::cout << "R" << '\n';
                        break;
                }
            }
        }
    }
    return collisionSide;
}

/*############  Servo Class Methods  ############*/
Servo::Servo() {}

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
    std::string json = serialRead(fd);
    return json;
}

void Servo::parseJsonVelocities(std::string json) {
    DynamicJsonBuffer jsonBuffer;
    JsonArray& array = jsonBuffer.parseArray(json);
    velocities.push_back(array[0]);
    velocities.push_back(array[1]);
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

void Servo::setServoVelocities(int fd, std::vector<int> &velVector){
    int n = 0;
    char buf[80];
    DynamicJsonBuffer jsonBuffer;
    JsonArray &velArray = jsonBuffer.createArray();
    velArray.add(velVector[0]);
    velArray.add(velVector[1]);
    int len = velArray.measureLength();
    velArray.printTo(buf);
    n = write(fd, buf, len);
    if (n <= 0) {
        std::cout << "No Bytes written..." << std::endl;
    }
}

//Non Class Functions
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

void publishData(zmq::socket_t& pub, std::string guiData) {
    zmq::message_t msg(guiData.size());
    std::memcpy(msg.data(), guiData.data(), guiData.size());
    pub.send(msg);
 
}
