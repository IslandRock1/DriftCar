//
// Created by Øystein Bringsli.
//

#ifndef CONTROLLER_MOTOR_HPP
#define CONTROLLER_MOTOR_HPP

#include <Arduino.h>

class Motor {
private:
    int pin0;
    int pin1;
    int pin2;
    int pin3;

    const int channel0 = 0;
    const int channel1 = 1;
    const int channel2 = 2;
    const int channel3 = 3;
    const int freq = 5000;
    const int resolution = 8;

public:
    Motor(int pin0, int pin1, int pin2, int pin3);
    void turnLeft(int speed);
    void turnRight(int speed);
};

Motor::Motor(int pin0, int pin1, int pin2, int pin3)
        :pin0(pin0), pin1(pin1), pin2(pin2), pin3(pin3) {

    ledcSetup(channel0, freq, resolution);
    ledcAttachPin(pin0, channel0);

    ledcSetup(channel1, freq, resolution);
    ledcAttachPin(pin1, channel1);

    ledcSetup(channel2, freq, resolution);
    ledcAttachPin(pin2, channel2);

    ledcSetup(channel3, freq, resolution);
    ledcAttachPin(pin3, channel3);
}

void Motor::turnLeft(int speed) {
    if (speed > 255) {speed = 255;}
    else if (speed < -255) {speed = -255;}

    if (speed < 0) {
        ledcWrite(channel0, 0);
        ledcWrite(channel1, abs(speed));
    } else {
        ledcWrite(channel1, 0);
        ledcWrite(channel0, abs(speed));
    }
}

void Motor::turnRight(int speed) {
    if (speed > 255) {speed = 255;}
    else if (speed < -255) {speed = -255;}

    if (speed < 0) {
        ledcWrite(channel2, 0);
        ledcWrite(channel3, abs(speed));
    } else {
        ledcWrite(channel3, 0);
        ledcWrite(channel2, abs(speed));
    }
}

#endif //CONTROLLER_MOTOR_HPP
