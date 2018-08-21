//Includes
#include "randomNavigation.hpp"
#include <chrono>
#include <thread>
#include <iostream>

processedData mainProcess(int fd, zmq::socket_t& pub, std::vector<float>& coords) {
    processedData data; //Struct Entity
    std::chrono::duration<float> dur; //measured time var

    //Class Entities//
    Sensor sensor;
    Servo servo;

    auto start = std::chrono::system_clock::now(); //Start Time Measurement

    //Data Processing Sensor//
    std::string json = sensor.requestSensorDataJsonString(fd); //request sensorData
    std::vector<int> flatVec = sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide(); //to be returned
    //std::vector<int> flatSensorData = sensor. //flatten sensorData
    json.clear(); //clear for reuse

    //Data Processing Servo//
    json = servo.requestServoDataJsonString(fd); //request velocities
    //servo.jsonToServoData(json); //parse jsonString
    servo.velocitiesInMeterPerSec(); //calc velocities in m/s
    json.clear(); //clear for reuse

    auto end = std::chrono::system_clock::now(); //End Time Measurement

    dur = end - start; //calc Time Difference

    std::vector<float> newCoords = servo.calculateCoords(coords, t); //calc Coords
    data.coords = newCoords; //to be returned

    json = guiDataToJsonString(flatVec, newCoords); //construct gui json string

    pub.send(&json, json.size()); //transfer guiData via ZMQ
}

void setVelocities(int fd, int velLeft, int velRight) {
    Servo servo;
    std::vector<int> velocities;
    velocities.push_back(velLeft);
    velocities.push_back(velRight);
    requestHandler(fd, servoWrite);
    writeToSerial(fd, servo.velocitiesToJson(velocities));
}

std::vector<int> rndmTurnVelocities() {
    std::vector<int> v;
    int n = 0, b = 0;
	srand((int)time(0));
	b = std::rand() % 2;
	for(size_t i = 0; i < 2; ++i) {
		n = 20 + (std::rand()) / (RAND_MAX/(100 - 20));
		v.push_back(n);
	}
	if(b == 0) {
		v.at(0) *= (-1);
	} else {
		v.at(1) *= (-1);
	}
	return v;
}

float rndmDurations() {
	srand(static_cast<int> (time(0)));
    float r = 0.5 + static_cast<float> (std::rand()) / (static_cast<float> (RAND_MAX/1.5 - 1));
	return r;
}

processedData wait(int fd, zmq::socket_t& pub, float waitTime, std::vector<float>& coords) {
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    processedData data;
    do {
        end = std::chrono::system_clock::now();
        data = mainProcess(fd, pub, coords);
        coords = data.coords;
        dur = end - start;
        dur.count();

    } while(dur.count() < waitTime);
    return data;
}

void hold(int fd) {
    setVelocities(fd, 0, 0);
}

processedData move(int fd, zmq::socket_t& pub, int velLeft, int velRight, float t, std::vector<float>& coords) {
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    processedData data;
    setVelocities(fd, velLeft, velRight);
    do {
        auto end = std::chrono::system_clock::now();
        data = mainProcess(fd, pub, coords);
        coords = data.coords;
        dur = end - start;
    } while(dur.count() < t);
    return data; 
}

processedData randomTurn(int fd, zmq::socket_t& pub, std::vector<float>& coords) {
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    float t = rndmDurations();
    std::vector<int> turnVelocities = rndmTurnVelocities();
    setVelocities(fd, turnVelocities.at(0), turnVelocities.at(1));
    processedData data;
    do {
        auto end = std::chrono::system_clock::now();
        data = mainProcess(fd, pub, coords);
        coords = data.coords;
        dur = end - start;
    } while(dur.count() < t);
    return data;
}
