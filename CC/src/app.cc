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
    //std::cout << json << '\n';
    sensor.jsonToSensorData(json); //parse jsonString
    data.collisionSide = sensor.detectCollisionSide(); //get collisionSide
    //std::vector<int> flatSensorData = sensor.flatData(); //flatten sensorData
    //json.clear(); //clear for reuse
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

void debug(int fd) {
    requestHandler(fd, sensorRead);
    std::string json = readFromUSB(fd);
    //std::cout << json << '\n';
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (root.success() == true) {
        std::cout << root["FL"][0] << '\n';
    }
}

std::string foo(int fd) {
    char buf[1048];
    int n = 0, nbytes = 0;
    std::string json;
    //spstd::cout << "fnctl: " << fcntl(fd, F_SETFL, 0) << '\n';
    do {
        //int val = fcntl(fd, F_GETFL, 0);
        //printf("file status = 0x%x\n", val);
        n = read(fd, buf+nbytes, sizeof buf - nbytes);
        if (n == -1) {
            std::cout << std::strerror(errno) << '\n';
        }
        //std::cout << n << '\n' << nbytes << '\n' << jsonBufLenLong-nbytes << '\n';
        if (n == 0) {
            continue;
        }
        nbytes++;
        std::cout << "buf: " << buf <<  '\n';
        std::cout << "sizeof buf: " << sizeof buf << '\n';
        break;
        if (buf[nbytes-1] == '}') {
            buf[nbytes] = '\0';
            json = buf;
            std::cout << json << '\n';
            break;
        }
    } while(nbytes <= jsonBufLenLong);
    return json;

}

int testread(int fd) {
    char buf[1048];
    int numread = 0;
    int nbytes = 0;
    int returnval;
    while (numread < nbytes - 1) {
    returnval = read(fd, buf + numread, 1);
    if ((returnval == -1) && (errno == EINTR))
        continue;
    if ( (returnval == 0) && (numread == 0) )
        return 0;
    if (returnval == 0)
        break;
    if (returnval == -1)
        return -1;
    numread++;
    //std::cout << buf[numread] << '\n';
    if (buf[numread-1] == '}') {
        buf[numread] = '\0';
        std::string json = buf;
        std::cout << json << '\n';
        return numread;
    }
    }
    errno = EINVAL;
    return -1;
}

std::string easyRead(int fd) {
    std::string json;
    int n = 0;
    char buf[bufLen];
    n = read(fd, buf, bufLen);
    if (n == -1 || n == 0) {
        std::cout << std::strerror(errno) << '\n';
    }
    buf[n] = '\0';
    json = buf;
    size_t jsonEndPos = json.find('}');
    json = json.erase(jsonEndPos+1, json.length());
    std::cout << json << '\n';
    return json;
}

int main(int argc, char *argv[]) {
    int fd = 0;
    std::vector<float> coords(3);
    processedData data;
    zmq::context_t context (1);
    zmq::socket_t pub(context, ZMQ_PUB);
    pub.bind("tcp://*:5555");
    fd = openPort("/dev/ttyACM0");
    std::string json;

    //std::string json = foo(fd);
    while (true) {
        //easyRead(fd);
        requestHandler(fd, sensorRead);
        //testread(fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //bar(fd);
        json = readFromUSB(fd);
        std::cout << json << '\n';
        //std::cout << json << '\n';
        //json = easyRead(fd);
    }
    //std::cout << json << '\n';
    //requestHandler(fd, sensorRead);
    /*while (true) {
        //sensorTest(fd);
        //requestHandler(fd, sensorRead);
        //readPort(fd);
        //testread(fd, buffer, 1024);
        //std::string json = readFromUSB(fd);
        //std::cout << json << '\n';
        //debug(fd);
        //servoTest(fd);
        //mainProcess(fd, pub, coords);
    }*/

    return 0;
}
