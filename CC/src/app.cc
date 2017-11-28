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
        std::this_thread::sleep_for(std::chrono::milliseconds(35));
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

void sensorTest(int fd) {
    processedData data;
    Sensor sensor;
    std::string json = sensor.requestSensorDataJsonString(fd); //request sensorData
    sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide(); //to be returned
    std::vector<int> flatSensorData = sensor.flatData(); //flatten sensorData
    json.clear(); //clear for reuse
}

void servoTest(int fd) {
    processedData data;
    Servo servo;
    std::string json = servo.requestServoDataJsonString(fd); //request velocities
    servo.jsonToServoData(json); //parse jsonString
    servo.velocitiesInMeterPerSec(); //calc velocities in m/s
    std::cout << json << '\n';
    json.clear(); //clear for reuse
}

int main(int argc, char *argv[]) {
    int fd = 0;
    std::vector<float> coords;
    processedData data;
    zmq::context_t context (1);
    zmq::socket_t pub(context, ZMQ_PUB);
    pub.bind("tcp://*:5555");
    fd = openPort("/dev/ttyACM0");
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        sensorTest(fd);
    }

    return 0;
}
