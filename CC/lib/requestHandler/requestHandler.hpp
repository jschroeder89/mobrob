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
#define jsonBufLenLong 1048
#define jsonBufLenShort 128
#define bufLen 256
#define collisionThreshold 3000
#define axisLength 0.15
#define revolutionsPerMinute 0.916
#define wheelDiameter 0.052


//Prototypes
//############  Non-Class Function Prototypes  ############
int openPort(char const *port);
std::string readFromUSB(int fd);
int writeToUSB(int fd, JsonObject& root);
void requestHandler(int fd, int op);
std::string guiDataToJsonString(std::vector<int>& sensorData, std::vector<float>& coords);
int readPort(int fd);
int bar(int fd);
//Classes
class Sensor {
public:
    Sensor();
    ~Sensor();
    std::string requestSensorDataJsonString(int fd);
    void jsonToSensorData(std::string json);
    enum class hasContact {frontLeft, frontRight, right, rear, left, none};
    hasContact detectCollisionSide();
    std::vector<int> flatData();
private:
    std::vector<std::vector<int> > sensorData;
};

class Servo {
public:
    Servo();
    ~Servo();
    //Write
    JsonObject& velocitiesToJson(std::vector<int>& velocities);
    //Read
    std::string requestServoDataJsonString(int fd);
    void jsonToServoData(std::string json);
    //Coordinates
    void velocitiesInMeterPerSec();
    std::vector<float> calculateCoords(std::vector<float>& previousCoords, float t);
private:
    std::vector<int> velocities;
    std::vector<float> velocitiesMeterPerSec;
};

#endif
