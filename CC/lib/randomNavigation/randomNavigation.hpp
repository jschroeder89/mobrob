#ifndef randomNavigation_HPP
#define randomNavigation_HPP

#include <zmq.hpp>
#include <requestHandler.hpp>
//Defines


//Prototypes
struct processedData {
    Sensor::hasContact collisionSide;
    std::vector<float> coords;
};
processedData mainProcess(int fd, zmq::socket_t& pub, std::vector<float>& coords);
processedData mainProcess(int fd, zmq::socket_t& pub);
std::vector<int> rndmTurnVelocities();
float rndmDurations();
bool collisionFront();
bool collisionRear();
void move(int fd, zmq::socket_t& pub, int velLeft, int velRight, float t);
void hold(int fd);
void wait(int fd, zmq::socket_t& pub, float waitTime);
void randomTurn(int fd, zmq::socket_t& pub);

#endif
