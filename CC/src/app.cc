//#include <requestHandler.hpp>
#include <zmq.hpp>

#define mainLoop true

int main(int argc, char *argv[]) {
    int fd = 0;
    zmq::context_t context (1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:3000");
    //fd = openPort("/dev/ttyACM0");
    while (mainLoop) {
        /* code */
    }
    //requestSensorData(fd);
    //requestServoData(fd);
    //setServoVelocities(fd, 200, 80);

    return 0;
}
