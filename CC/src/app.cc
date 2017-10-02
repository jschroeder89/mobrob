#include <requestHandler.hpp>
#include <randomNavigation.hpp>
#include <iostream>
#include <zmq.hpp>
#include <chrono>
#include <thread>

Sensor::hasContact Sensor::collisionSide;

void navigate(int fd, zmq::socket_t& pub);
void capture();
void foo(int fd, zmq::socket_t& pub);

void navigate(int fd, zmq::socket_t& pub) {

}

void capture() {

}

void foo(int fd, zmq::socket_t& pub) {
    std::string json = GUIData();
    const char* buffer = json.c_str();
    pub.send(buffer, json.size());
}

int main(int argc, char *argv[]) {
    int fd = 0;
    zmq::context_t context (1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5555");
    fd = openPort("/dev/ttyACM0");

    while (true) {
        requestSensorData(fd);
        foo(fd, publisher);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}
