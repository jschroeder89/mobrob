//Includes
#include "randomNavigation.hpp"
#include <chrono>
#include <thread>
#include <iostream>

processedData mainProcess(int fd, zmq::socket_t& pub, std::vector<float>& coords) {
    Sensor sensor;
    Servo servo;
    processedData data;
    float t = 0;
    std::chrono::duration<float> dur;
    auto start = std::chrono::system_clock::now();

    std::string json = sensor.requestSensorDataJsonString(fd);       //request sensorData
    std::vector<int> flatSensorData = sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide();                  //get collisionSide

    json = servo.requestServoDataJsonString(fd); //request velocities
    servo.parseJsonVelocities(json);                         //parse jsonString
    servo.velocitiesInMeterPerSec();                         //calc velocities in m/s

    auto end = std::chrono::system_clock::now();

    dur = end - start;
    t = dur.count();
    data.coords = servo.calculateCoords(coords, t);

    std::string guiData = guiDataToJsonString(flatSensorData, coords);
    zmq::message_t msg(guiData.size());
    std::memcpy(msg.data(), guiData.data(), guiData.size());
    pub.send(msg);
    return data;
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
        dur = end - start;
        dur.count();
    } while(dur.count() < waitTime);
    return data;
}

processedData hold(int fd, zmq::socket_t &pub, std::vector<float> &coords) {
    Servo s;
    processedData data;
    std::vector<int> hold{0, 0};
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, hold);
    do
    {
        end = std::chrono::system_clock::now();
        data = mainProcess(fd, pub, coords);
        coords = data.coords;
        dur = end - start;
    } while (dur.count() < 2.0);
    return data;
}

processedData moveBack(int fd, zmq::socket_t& pub, std::vector<float>& coords) {
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    processedData data;
    Servo s;
    std::vector<int> backwards{-50, -50};
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, backwards);
    do {
        end = std::chrono::system_clock::now();
        data = mainProcess(fd, pub, coords);
        dur = end - start;
    } while(dur.count() < 1.0);
    return data; 
}

processedData randomTurn(int fd, zmq::socket_t& pub, std::vector<float>& coords) {
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    float t = rndmDurations();
    std::vector<int> turnVelocities = rndmTurnVelocities();
    Servo s;
    requestHandler(fd, servoWrite);
    s.setServoVelocities(fd, turnVelocities);
    processedData data;
    do {
        end = std::chrono::system_clock::now();
        data = mainProcess(fd, pub, coords);
        dur = end - start;
    } while(dur.count() < t);
    return data;
}
