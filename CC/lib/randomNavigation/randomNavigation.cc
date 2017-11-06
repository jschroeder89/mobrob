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
    sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide(); //to be returned
    std::vector<int> flatSensorData = sensor.flatData(); //flatten sensorData
    sensor.~Sensor(); //destroy Entity
    json.clear(); //clear for reuse

    //Data Processing Servo//
    json = servo.requestServoDataJsonString(fd); //request velocities
    servo.jsonToServoData(json); //parse jsonString
    servo.velocitiesInMeterPerSec(); //calc velocities in m/s
    json.clear(); //clear for reuse

    auto end = std::chrono::system_clock::now(); //End Time Measurement

    dur = end - start; //calc Time Difference
    float t = dur.count(); //convert Time Difference to float

    std::vector<float> newCoords = servo.calculateCoords(coords, t); //calc Coords
    data.coords = newCoords; //to be returned
    servo.~Servo(); //destroy Entity

    json = guiDataToJsonString(flatSensorData, newCoords); //construct gui json string

    pub.send(&json, json.size()); //transfer guiData via ZMQ
    return data; //return processedData struct
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
    float r = 1 + static_cast<float> (std::rand()) / (static_cast<float> (RAND_MAX/2 - 1));
	return r;
}

void wait(int fd, zmq::socket_t& pub, float waitTime) {
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    do {
        end = std::chrono::system_clock::now();
        //dataLoop(fd, pub);
        dur = end - start;
        dur.count();

    } while(dur.count() < waitTime);
}

void hold(int fd) {

}

void move(int fd, zmq::socket_t& pub, int velLeft, int velRight, float t) {
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    do {
        auto end = std::chrono::system_clock::now();
        //dataLoop(fd, pub);
        dur = end - start;
    } while(dur.count() < t);
}

void randomTurn(int fd, zmq::socket_t& pub) {
    float dur = rndmDurations();
    std::vector<int> turnVelocities;
    turnVelocities = rndmTurnVelocities();
    move(fd, pub, turnVelocities.at(0), turnVelocities.at(1), dur);
}
