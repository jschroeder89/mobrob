//Includes
#include "randomNavigation.hpp"
#include <requestHandler.hpp>
#include <chrono>

void dataLoop(int fd, zmq::socket_t& pub) {
    std::string json = GUIData();
    const char* buffer = json.c_str();
    auto start = std::chrono::system_clock::now();
    requestHandler(fd, servoRead);
    requestHandler(fd, sensorRead);
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    float t = dur.count();
    getCoordinates(t);
    pub.send(buffer, json.size());
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
