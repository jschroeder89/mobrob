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


    setServoVelocities(fd, 5, 5);
    //move(fd, 10, 10, 5, publisher);
    hold(fd);
    while(1) {
        requestSensorData(fd);
        requestServoData(fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    /*while (true) {
        requestSensorData(fd);
        foo(fd, publisher);
        switch (Sensor::collisionSide) {
            case Sensor::hasContact::frontLeft:
                std::cout << "FL" << std::endl;
                break;
            case Sensor::hasContact::frontRight:
                std::cout << "FR" << std::endl;
                break;
            case Sensor::hasContact::left:
                std::cout << "L" << std::endl;
                break;
            case Sensor::hasContact::rear:
                std::cout << "B" << std::endl;
                break;
            case Sensor::hasContact::right:
                std::cout << "R" << std::endl;
                break;
            default:
                Sensor::collisionSide = Sensor::hasContact::none;
                break;
        }
    }*/

    return 0;
}
