#include <unistd.h>
#include <requestHandler.hpp>
#include <randomNavigation.hpp>
#include <iostream>
#include <zmq.hpp>
#include <chrono>
#include <thread>


processedData sensorTest(int fd, processedData& data);
processedData servoTest(int fd, processedData& data);
void capture();
void servoVels(int fd);
void randomWalk(int fd, zmq::socket_t &pub, std::vector<float> &coords);

void randomWalk(int fd, zmq::socket_t &pub, std::vector<float>& prevCoords) {
    Servo s;
    processedData data;
    data.coords = prevCoords;
    std::vector<int> initVel{70, 70};
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, initVel);
    
    while(true){
        data = mainProcess(fd, pub, data.coords);
        //std::this_thread::sleep_for(std::chrono::milliseconds(35));
    }

    if (data.collisionSide == Sensor::hasContact::none) {
        data = hold(fd, pub, data.coords);
        data = wait(fd, pub, 1.0, data.coords);
        data = moveBack(fd, pub, data.coords);
        data = hold(fd, pub, data.coords);
        data = wait(fd, pub, 1.0, data.coords);
        data = randomTurn(fd, pub, data.coords);
        data = hold(fd, pub, data.coords);
        data = wait(fd, pub, 1.0, data.coords);
    }
    
    randomWalk(fd, pub, data.coords);
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
}

processedData sensorTest(int fd, processedData& data) {
    Sensor sensor;
    std::string json = sensor.requestSensorDataJsonString(fd); //request sensorData
    //std::cout << json << '\n';
    std::vector<int> flatSensorData = sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide(); //get collisionSide
    return data;
}

processedData servoTest(int fd, processedData& data, std::vector<float> &prevCoords, float t) {
    Servo servo;
    std::string json = servo.requestServoDataJsonString(fd); //request velocities
    servo.parseJsonVelocities(json); //parse jsonString
    servo.velocitiesInMeterPerSec(); //calc velocities in m/s
    data.coords = servo.calculateCoords(prevCoords, t);
    return data;
}

void initialVel(int fd, std::vector<int>& initVel) {
    Servo servo;
    requestHandler(fd, servoWrite);
    servo.setServoVelocities(fd, initVel);
}

void servoHold(int fd) {
    Servo s;
    std::vector<int> hold{0,0};
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, hold);
}

void moveBack(int fd) {
    Servo s;
    std::vector<int> backwards{-20,-20};
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, backwards);
}

void rndTurn(int fd) {
    Servo s;
    std::vector<int> holdVel{0,0};
    std::vector<int> turnVel = rndmTurnVelocities();
    float turnDuration = rndmDurations();
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, turnVel);
    do {
        end = std::chrono::system_clock::now();
        dur = end - start;
        //dur.count();
    } while(dur.count() < turnDuration);
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, holdVel);
}

processedData dataProcessing(int fd, std::vector<float>& coords) {
    Servo servo;
    Sensor sensor;

    /*SERVO*/
    std::string jsonVels = servo.requestServoDataJsonString(fd);
    
}


void naivRandomWalk(int fd) {
    processedData d;
    //std::vector<float> coords(3);
    d.collisionSide = Sensor::hasContact::none; 
    std::vector <int> initVel{30,30};
    initialVel(fd, initVel);
    while(true){
        //d = servoTest(fd, d);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        d = sensorTest(fd, d);
        if (d.collisionSide != Sensor::hasContact::none) {
            servoHold(fd);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            moveBack(fd);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            rndTurn(fd);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            servoHold(fd);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        naivRandomWalk(fd);    
    }
}

void guiTest(int fd, std::vector<float> coords, zmq::socket_t& pub) {
    Sensor s;
    std::string json = s.requestSensorDataJsonString(fd);
    std::vector<int> v = s.jsonToSensorData(json);
    std::string guiData = guiDataToJsonString(v, coords);
    zmq::message_t msg(guiData.size());
    std::memcpy(msg.data(), guiData.data(), guiData.size());
    pub.send(msg);
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
    std::vector <int> holdVector{0,0};
    Servo servo;

    randomWalk(fd, pub, coords);
    //naivRandomWalk(fd);
    guiTest(fd, coords, pub);

    return 0;
}
