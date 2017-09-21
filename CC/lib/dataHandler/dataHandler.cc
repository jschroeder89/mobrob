#include "dataHandler.hpp"

void detectCollisionSide(std::vector<std::vector<int> >& v) {
    Sensor::collisionSide = Sensor::hasContact::none;

    for (int i = 0; i < 8; i++) {
        if (v.at(0).at(i) >= collisionThreshold
        && Sensor::collisionSide == Sensor::hasContact::none) {
            Sensor::collisionSide = Sensor::hasContact::frontLeft;
            break;
        } else {
            Sensor::collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(1).at(i) >= collisionThreshold
        && Sensor::collisionSide == Sensor::hasContact::none) {
            Sensor::collisionSide = Sensor::hasContact::frontRight;
            break;
        } else {
            Sensor::collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(2).at(i) >= collisionThreshold
        && Sensor::collisionSide == Sensor::hasContact::none) {
            Sensor::collisionSide = Sensor::hasContact::right;
            break;
        } else {
            Sensor::collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(3).at(i) >= collisionThreshold
        && Sensor::collisionSide == Sensor::hasContact::none) {
            Sensor::collisionSide = Sensor::hasContact::rear;
            break;
        } else {
            Sensor::collisionSide = Sensor::hasContact::none;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (v.at(4).at(i) >= collisionThreshold
        && Sensor::collisionSide == Sensor::hasContact::none) {
            Sensor::collisionSide = Sensor::hasContact::left;
            break;
        } else {
            Sensor::collisionSide = Sensor::hasContact::none;
        }
    }
}

void copySensorVector(std::vector<int>& v) {
    Sensor::flatSensorData = v;
}

void convertTicksToVelocities(std::vector<int>& v) {
    Servo::velocitiesMeterPerSec.reserve(2);

    for (int i = 0; i < 2; i++) {
        Servo::velocitiesMeterPerSec.at(i) =
        wheelDiameter * M_PI * revolutionsPerMinute/60 * v.at(i);
    }
}

void getCoordinates(float t) {
    //Servo::coords.reserve(3);
    float distanceLeft = t * Servo::velocitiesMeterPerSec.at(0);
    float distanceRight = t * Servo::velocitiesMeterPerSec.at(1);
    float distanceMiddle = (distanceLeft + distanceRight) / 2;
    float distanceDif = (distanceRight - distanceLeft) / 2 * axisLength;

    float theta = (distanceRight - distanceLeft)/axisLength + Servo::coords.at(2);
    Servo::coords.at(2) = theta;

    float x = distanceMiddle * cosf(Servo::coords.at(2) * distanceDif) + Servo::coords.at(0);
    Servo::coords.at(0) = x;

    float y = distanceMiddle * sinf(Servo::coords.at(2) * distanceDif) + Servo::coords.at(1);
    Servo::coords.at(1) = y;
}

void convertGuiDataToJson(std::vector<int> sensorData, std::vector<float> coords) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["zmq"] = "message";
    JsonArray& sensor = root.createNestedArray("sensor");
    JsonArray& path = root.createNestedArray("path");

    for (size_t i = 0; i < sensorData.size(); i++) {
        sensor.add(sensorData.at(i));
    }
        for (size_t i = 0; i < coords.size(); i++) {
            path.add(coords.at(i));
        }

    convertJsonToCharBuffer(root);
}

char convertJsonToCharBuffer(JsonObject& root) {
    int bufferSize = 0;
    char buffer[bufferSize];

    bufferSize = root.measureLength();
    root.printTo(buffer, sizeof buffer);

    return *buffer;
}
