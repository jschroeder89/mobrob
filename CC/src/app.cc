  #include <unistd.h>
#include <requestHandler.hpp>
#include <randomNavigation.hpp>
#include <iostream>
#include <zmq.hpp>
#include <chrono>
#include <thread>


processedData sensorTest(int fd, processedData& data);
processedData servoTest(int fd, processedData& data);
processedData componentsCheck(int fd, zmq::socket_t& pub, processedData& data);
processedData navigate(int fd, zmq::socket_t& pub, std::vector<float>& previousCoords);
void capture();
void servoVels(int fd);


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

void servoVels(int fd, std::vector<int>& velVector){
    int n = 0;
    char buf[80];
    char newLine[1] = {'\n'};
    DynamicJsonBuffer jsonBuffer;
    JsonArray& velArray = jsonBuffer.createArray();
    velArray.add(velVector[0]);
    velArray.add(velVector[1]);
    int len = velArray.measureLength();        
    velArray.printTo(buf);
    n = write(fd, buf, len);
    //write(fd, newLine, sizeof newLine);
}

processedData sensorTest(int fd, processedData& data) {
    Sensor sensor;
    std::string json = sensor.requestSensorDataJsonString(fd); //request sensorData
    std::cout << json << '\n';
    std::vector<int> flatSensorData = sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide(); //get collisionSide
    return data;
}

processedData servoTest(int fd, processedData& data) {
    Servo servo;
    std::string json = servo.requestServoDataJsonString(fd); //request velocities
    //std::cout << json << '\n';
    servo.jsonToServoData(json); //parse jsonString
    servo.velocitiesInMeterPerSec(); //calc velocities in m/s

    return data;
}

processedData componentsCheck(int fd, zmq::socket_t& pub, processedData& data) {
    setVelocities(fd, 5, 5);
    

    data = sensorTest(fd, data);
    data = servoTest(fd, data);
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
    std::vector <int> velVector{2,2};
    
    requestHandler(fd, servoWrite);
    servoVels(fd, velVector);

    while (true) {
        requestHandler(fd, sensorRead);
        requestHandler(fd, servoRead);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //data = sensorTest(fd, data);
        //data = servoTest(fd, data);
        /*if (data.collisionSide != Sensor::hasContact::none) {
            hold(fd);
            break;
        }*/
    }
    std::cout << "DONE" << std::endl;
    return 0;
}
