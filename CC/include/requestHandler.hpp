#ifndef requestHandler_HPP
#define requestHandler_HPP

//Includes
#include <ArduinoJson.h>
#include <vector>
#include <zmq.hpp>

//Defines
#define sensorRead 1
#define sensorReadByte '1'
#define servoRead 2
#define servoReadByte '2'
#define servoWrite 3
#define servoWriteByte '3'

//Prototypes
int openPort(char const *port);
void requestHandler(int fd, int op);
std::string guiDataToJsonString(std::vector<int>& sensorData, std::vector<float>& coords);

class Sensor {
public:
    Sensor();
    ~Sensor();
    std::string requestSensorDataJsonString(int fd);
    std::vector<int> jsonToSensorData(std::string json);
    enum class collision_detected {
        frontLeft,
        frontRight,
        right,
        rear,
        left,
        none
    };
    collision_detected detectCollisionSide();
private:
    std::vector<std::vector<int> > sensorData;
};

class Servo {
public:
    Servo();
    ~Servo();
    //Write
    void setServoVelocities(int fd, std::vector<int> &velVector);
    JsonObject &velocitiesToJson(std::vector<int> &velocities);
    //Read
    std::string requestServoDataJsonString(int fd);
    void parseJsonVelocities(std::string json);
    //Coordinates
    void velocitiesInMeterPerSec();
    std::vector<float> calculateCoords(std::vector<float>& previousCoords, float t);
private:
    std::vector<int> velocities;
    std::vector<float> velocitiesMeterPerSec;
};

#endif
