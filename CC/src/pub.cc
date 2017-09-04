#include <zmq.hpp>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <iostream>
#include <boost/thread/thread.hpp>

int main(int argc, char *argv[]) {
    int bufferSize;
    char buf[bufferSize];

    zmq::context_t context (1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:3000");

    DynamicJsonBuffer jsonBuffer;
    //StaticJsonBuffer<512> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["message"] = "sensorData";
    JsonArray& sensorFrontLeft = root.createNestedArray("FL");
    sensorFrontLeft.add(2309);
    sensorFrontLeft.add(3098);
    bufferSize = root.measureLength();
    root.printTo(buf, sizeof buf);


    while (1) {
        publisher.send(buf, bufferSize);
        std::cout << root << std::endl << bufferSize << std::endl;
        boost::this_thread::sleep_for( boost::chrono::milliseconds(500) );
    }
    return 0;
}
