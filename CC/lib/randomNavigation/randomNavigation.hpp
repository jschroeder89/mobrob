#ifndef randomNavigation_HPP
#define randomNavigation_HPP

#include <zmq.hpp>
#include <requestHandler.hpp>
//Defines


//Prototypes
struct processedData {
    std::vector<int> sensorVec;
    Sensor::hasContact collisionSide;
    std::vector<float> coords;
};
processedData mainProcess(int fd, zmq::socket_t& pub, std::vector<float>& coords);
processedData mainProcess(int fd, zmq::socket_t& pub);
processedData move(int fd, zmq::socket_t& pub, int velLeft, int velRight, float t, std::vector<float>& coords);
processedData wait(int fd, zmq::socket_t& pub, float waitTime, std::vector<float>& coords);
processedData randomTurn(int fd, zmq::socket_t& pub, std::vector<float>& coords);
std::vector<int> rndmTurnVelocities();
float rndmDurations();
void setVelocities(int fd, int velLeft, int velRight);
void hold(int fd);

#endif
