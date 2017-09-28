//Includes
#include "randomNavigation.hpp"
#include <requestHandler.hpp>
#include <chrono>

void dataLoop(int fd, zmq::socket_t& pub) {
    auto start = std::chrono::system_clock::now();
    requestHandler(fd, servoRead);
    requestHandler(fd, sensorRead);
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    float t = dur.count();
    getCoordinates(t);
    char buf = {0};
    buf = GUIData();
    pub.send(&buf, sizeof buf);
}

void moveForward(int fd) {
    setServoVelocities(fd, 100, 100);
}

void moveBackward() {

}

void randomTurn() {

}

void randomNav() {

}
