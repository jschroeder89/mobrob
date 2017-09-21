#ifndef dataHandler_HPP
#define dataHandler_HPP

//Includes
#include "/home/johann/github/mobrob/CC/lib/requestHandler/requestHandler.hpp"
#include <math.h>

//Defines
#define collisionThreshold 3000
#define axisLength 0.15
#define revolutionsPerMinute 0.916
#define wheelDiameter 0.052

//Prototypes
void detectCollisionSide(std::vector<std::vector<int> >& v);
void copySensorVector(std::vector<int>& v);
void convertTicksToVelocities(std::vector<int>& v);
void getCoordinates(float t);
char convertJsonToCharBuffer(JsonObject& root);

class Sensor {
    public:
        enum class hasContact {
            frontLeft,
            frontRight,
            right,
            rear,
            left,
            none
        };
        static hasContact collisionSide;
        static std::vector<int> flatSensorData;
};

class Servo {
    public:
        static std::vector<float> velocitiesMeterPerSec;
        static std::vector<float> coords;
};

#endif
