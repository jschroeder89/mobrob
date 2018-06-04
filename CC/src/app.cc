#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <requestHandler.hpp>
#include <randomNavigation.hpp>
#include <iostream>
#include <zmq.hpp>
#include <chrono>
#include <thread>


processedData navigate(int fd, zmq::socket_t& pub, std::vector<float>& previousCoords);
void capture();

processedData navigate(int fd, zmq::socket_t& pub, std::vector<float>& previousCoords) {
    processedData data;
    do {
        data = mainProcess(fd, pub, data.coords);
        //std::this_thread::sleep_for(std::chrono::milliseconds(35));
    } while (data.collisionSide == Sensor::hasContact::none);

    hold(fd);
    data = wait(fd, pub, 1, data.coords);
    data = move(fd, pub, -25, -25, 2, data.coords);
    data = wait(fd, pub, 1, data.coords);
    data = randomTurn(fd, pub, data.coords);
    data = wait(fd, pub, 1, data.coords);

    return data;
}

void capture() {

}

processedData sensorTest(processedData& data) {
    Sensor sensor;
    std::string json = sensor.requestSensorDataJsonString(data.fd); //request sensorData
    std::cout << json << '\n';
    std::vector<int> flatSensorData = sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide(); //get collisionSide
    return data;
}

processedData servoTest(processedData& data) {
    Servo servo;
    std::string json = servo.requestServoDataJsonString(data.fd); //request velocities
    servo.jsonToServoData(json); //parse jsonString
    servo.velocitiesInMeterPerSec(); //calc velocities in m/s
    std::cout << json << '\n';
    return data;
}

int main(int argc, char *argv[]) {
    processedData data;
    std::vector<float> coords(3);
    zmq::context_t context (1);
    zmq::socket_t pub(context, ZMQ_PUB);
    pub.bind("tcp://*:5555");
    int fd = openPort("/dev/ttyACM0");
    data.coords = coords;
    data.collisionSide = Sensor::hasContact::none; 
    std::string json;
    
    while (true) {
        //data = sensorTest(data);
        requestHandler(fd, sensorRead);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        json = serialRead(fd);
        std::cout << json << '\n';
    }

    return 0;
}
