#include "requestHandler.hpp"

int openPort(char const *port) {
    int fd = open(port, O_RDWR | O_NDELAY);
        if  (fd == -1) {
            perror("Port not found");
            return -1;
        }
        /*struct termios options;

        tcgetattr(fd, &options);
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_iflag &= ~(BRKINT|ICRNL|ISTRIP|IXON);
        options.c_oflag = 0;
        options.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
        tcflush( fd, TCIFLUSH );
        tcsetattr(fd, TCSANOW, &options);*/
    return fd;
}

void requestHandler(int fd, int op) {
    char byte;

    switch (op) {
        case sensorRead:
        byte = sensorReadByte;
        break;
            case servoRead:
            byte = servoReadByte;
            break;
                case servoWrite:
                byte = servoWriteByte;
                break;
    }
    int n = write(fd, &byte, sizeof byte);
    //std::cout << "Written " << n << " Byte(s)" << std::endl;
    //std::cout << byte << std::endl;
    if (n <= 0) {
        std::cout << "No bytes writen!" << std::endl;
    }
}

int readFromUSB(int fd, int op) {
    char buf[bufLen];
    int n = 0, nbytes = 0, i = 0;
    std::string json;



    do {
        n = read(fd, buf+nbytes, bufLen-nbytes);

        if (buf[nbytes-1] == '}') {
            buf[nbytes] == '\0';
            json = buf;
            break;
        }
        nbytes +=n;
    } while(nbytes <= bufLen);
    size_t jsonStartPos = json.find('{');
    json = json.erase(0, jsonStartPos);
    size_t jsonEndPos = json.find('}');
    json = json.erase(jsonEndPos+1, json.length());
    std::cout << json[json.length()-1] << std::endl << json << std::endl;
    if (op == sensorRead) {
        jsonSensorParser(json);
    } else if (op == servoRead) {
        jsonServoParser(json);
    }
    return 0;
}

void jsonSensorParser(std::string json) {

    //DynamicJsonBuffer jsonBuffer;
    StaticJsonBuffer<jsonBufLen> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    std::cout << root["FL"][0] << std::endl;


    std::vector<int> FL, FR, L, R, B;
    std::vector<std::vector<int> > sensorData;

    /*for (size_t i = 0; i < 8; i++) {
        FL.push_back(root["FL"][i]);
        FR.push_back(root["FR"][i]);
        L.push_back(root["L"][i]);
        B.push_back(root["B"][i]);
        R.push_back(root["R"][i]);
    }
    sensorData.push_back(FL);
    sensorData.push_back(FR);
    sensorData.push_back(L);
    sensorData.push_back(B);
    sensorData.push_back(R);
*/
}

void jsonServoParser(std::string json) {
    StaticJsonBuffer<jsonBufLen> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    std::vector<int> servoData;
    servoData.push_back(root["velLeft"]);
    servoData.push_back(root["velRight"]);

}

void writeToUSB(int fd, JsonObject& root, int bufferSize) {
    char buffer[bufferSize+1];
    root.printTo(buffer, sizeof buffer);
    int n = write(fd, &buffer, sizeof buffer);
    if (n > 0) {
        std::cout << "Written " << n << " Byte(s)" << std::endl;
        std::cout << buffer << std::endl;
    }
}

void setVelocities(int fd, int velLeft, int velRight) {
    convertVelocitiesToJson(fd, velLeft, velRight);
}

void convertVelocitiesToJson(int fd, int velLeft, int velRight) {
    StaticJsonBuffer<jsonBufLen> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root["data"] = "servoVels";
    root["velLeft"] = velLeft;
    root["velRight"] = velRight;

    int bufferSize = root.measureLength();

    writeToUSB(fd, root, bufferSize);
}

void requestSensorData(int fd) {
    requestHandler(fd, sensorRead);
    readFromUSB(fd, sensorRead);
}

void requestServoData(int fd) {
    requestHandler(fd, servoRead);
    readFromUSB(fd, servoRead);
}

void setServoVelocities(int fd, int velLeft, int velRight) {
    requestHandler(fd, servoWrite);
    setVelocities(fd, velLeft, velRight);
}

int main(int argc, char *argv[]) {
    int fd = 0;
    fd = openPort("/dev/ttyACM0");

    //requestSensorData(fd);
    requestServoData(fd);
    //setServoVelocities(fd, 0, 0);

    return 0;
}
