#ifndef randomNavigation_HPP
#define randomNavigation_HPP

#include <zmq.hpp>

//Defines


//Prototypes
void dataLoop(int fd, zmq::socket_t& pub);
std::vector<int> rndmTurnVelocities();
float rndmDurations();
bool collisionFront();
bool collisionRear();
void move(int fd, int velLeft, int velRight, float t, zmq::socket_t& pub);
void hold(int fd);
void randomTurn(int fd, zmq::socket_t& pub);

#endif
