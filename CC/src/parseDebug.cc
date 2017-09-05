#include <ArduinoJson.h>
#include <iostream>

void create();
void parse(char* json);

void create() {
    char json[300];
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["data"] = "sensor";
    JsonArray& sensorFrontLeft = root.createNestedArray("FL");
    JsonArray& sensorFrontRight = root.createNestedArray("FR");
    sensorFrontLeft.add(4949);
    sensorFrontLeft.add(2355);
    sensorFrontRight.add(2545);
    root.printTo(json);
    std::cout << json << std::endl;
    parse(&json[0]);
}

void parse(char* json) {
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success()) {
        std::cout << "NOPE" << std::endl;
    }
    std::cout << root.success() << std::endl;
    for (size_t i = 0; i < 2; i++) {
        std::cout << root["FL"][i] << std::endl;
    }
}

int main(int argc, char *argv[]) {
    create();
    return 0;
}
