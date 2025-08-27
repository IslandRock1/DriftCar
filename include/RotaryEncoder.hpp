
#ifndef CONTROLLER_ROTARYENCODER_HPP
#define CONTROLLER_ROTARYENCODER_HPP

#include <Arduino.h>

class RotaryEncoder {
private:
    int PIN_SW;
    int PIN_DT;
    int PIN_CLK;
    int lastCLK = HIGH;
public:
    RotaryEncoder(int PIN_SW, int PIN_DT, int PIN_CLK);
    void readRotary();

    int value = 0;
    bool button = false;
    bool resetOnButton = true;
};

RotaryEncoder::RotaryEncoder(int PIN_SW, int PIN_DT, int PIN_CLK)
    : PIN_SW(PIN_SW), PIN_DT(PIN_DT), PIN_CLK(PIN_CLK) {
    pinMode(PIN_SW, INPUT_PULLUP);
    pinMode(PIN_DT, INPUT);
    pinMode(PIN_CLK, INPUT);
}

void RotaryEncoder::readRotary() {
    int currentCLK = digitalRead(PIN_CLK);
    if (currentCLK != lastCLK && currentCLK == HIGH) { // Only act on rising edge
        if (digitalRead(PIN_DT) != currentCLK) {
            value += 1;
        } else {
            value -= 1;
        }
    }
    lastCLK = currentCLK;

    if (digitalRead(PIN_SW) == LOW) {
        button = true;
        if (resetOnButton) {value = 0;}
    } else {
        button = false;
    }
}

#endif //CONTROLLER_ROTARYENCODER_HPP
