#include <requestHandler.hpp>
#include <randomNavigation.hpp>
#include <iostream>
#include <zmq.hpp>
#include <chrono>
#include <thread>

void navigate(int fd, zmq::socket_t& pub);

void navigate(int fd, zmq::socket_t& pub) {

}

int main(int argc, char *argv[]) {
    int fd = 0;
    zmq::context_t context (1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5555");
    fd = openPort("/dev/ttyACM0");
    //requestHandler(fd, sensorRead);
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        navigate(fd, publisher);
        std::cout << "YEP" << std::endl;
    }

    return 0;
}
