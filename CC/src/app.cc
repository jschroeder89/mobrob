#include "/home/johann/github/mobrob/CC/lib/requestHandler/requestHandler.hpp"
#include "/home/johann/github/mobrob/CC/lib/dataHandler/dataHandler.hpp"
#include <zmq.hpp>


void dataLoop(int fd) {

}

int main(int argc, char *argv[]) {
    int fd = 0;
    zmq::context_t context (1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:3000");
    fd = openPort("/dev/ttyACM0");
    while (true) {
        /* code */
    }
    //requestSensorData(fd);
    //requestServoData(fd);
    //setServoVelocities(fd, 200, 80);

    return 0;
}
