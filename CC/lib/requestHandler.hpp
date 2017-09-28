#ifndef requestHandler_HPP
#define requestHandler_HPP

//Includes
#include <ArduinoJson.h>
#include <vector>

//Defines
#define sensorRead 1
#define sensorReadByte '1'
#define servoRead 2
#define servoReadByte '2'
#define servoWrite 3
#define servoWriteByte '3'
#define bufLen 1024
#define jsonBufLen 1024
#define collisionThreshold 3000
#define axisLength 0.15
#define revolutionsPerMinute 0.916
#define wheelDiameter 0.052


//Prototypes
int openPort(char const *port);
int readFromUSB(int fd);
void requestHandler(int fd, int op);
void convertVelocitiesToJson(int fd, int velLeft, int velRight);
void writeToUSB(int fd, JsonObject& root);
void setVelocities(int fd, int velLeft, int velRight);
void setServoVelocities(int fd, int velLeft, int velRight);
void jsonSensorParser(std::string json);
void jsonServoParser(std::string json);
void detectCollisionSide(std::vector<std::vector<int> >& v);
void copySensorVector(std::vector<int>& v);
void convertTicksToVelocities(std::vector<int>& v);
void getCoordinates(float t);
char GUIData();

class Sensor {
    public:
        enum class hasContact {
            frontLeft,
            frontRight,
            right,
            rear,
            left,
            none
        };
        static hasContact collisionSide;
        static std::vector<int> sensorDataFlat;
};

class Servo {
    public:
        static std::vector<float> velocitiesMeterPerSec;
        static std::vector<float> coords;
};


#endif
