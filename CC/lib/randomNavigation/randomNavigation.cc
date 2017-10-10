//Includes
#include "randomNavigation.hpp"
#include <requestHandler.hpp>
#include <chrono>
#include <iostream>

void dataLoop(int fd, zmq::socket_t& pub) {
    auto start = std::chrono::system_clock::now();
        requestHandler(fd, servoRead);
        requestHandler(fd, sensorRead);
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    float t = dur.count();
    getCoordinates(t);
    //std::string json = GUIData();
    //const char* buffer = json.c_str();
    //pub.send(buffer, json.size());
}

std::vector<int> rndmTurnVelocities() {
	srand((int)time(0));
	int n = 0, b = 0;
	std::vector<int> v;
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
	float r = 1 + static_cast<float> (std::rand()) / (static_cast<float> (RAND_MAX/2.50));
	return r;
}

bool collisionFront() {
    if (Sensor::collisionSide == Sensor::hasContact::frontLeft
    || Sensor::collisionSide == Sensor::hasContact::frontRight) {
        return true;
    } else {
        return false;
    }
}

bool collisionRear() {
    if (Sensor::collisionSide == Sensor::hasContact::rear) {
        return true;
    } else {
        return false;
    }
}

void hold(int fd) {
    setServoVelocities(fd, 0, 0);
}

void move(int fd, int velLeft, int velRight, float t, zmq::socket_t& pub) {
    setServoVelocities(fd, velLeft, velRight);
    auto start = std::chrono::system_clock::now();
    std::chrono::duration<float> dur;
    do {
        auto end = std::chrono::system_clock::now();
        dataLoop(fd, pub);
        dur = end - start;
    } while(dur.count() < t);
}

void randomTurn(int fd, zmq::socket_t& pub) {
    float dur = rndmDurations();
    std::vector<int> turnVelocities = rndmTurnVelocities();
    move(fd, turnVelocities.at(0), turnVelocities.at(1), dur, pub);
}
