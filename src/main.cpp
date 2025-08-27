#include <Arduino.h>

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "RotaryEncoder.hpp"
#include "Motor.hpp"
#include "Buzzer.hpp"

RotaryEncoder* encoderSpeed = nullptr;
RotaryEncoder* encoderSteering = nullptr;

Motor *motor = nullptr;
Buzzer *buzzer = nullptr;

std::array<uint8_t, 6> adress0{168, 66, 227, 174, 248, 156};
std::array<uint8_t, 6> adress1{168, 66, 227, 174, 240, 228};
esp_now_peer_info_t peerInfo;

int selfID = -1;
// 0: Car
// 1: Controller

typedef struct DriftData {
    int id;
    int speed ;
    int steering;
    int music;
} DriftData;

DriftData driftData;
DriftData recievingData;

void sendData(int speed, int steering, int music) {
    driftData.id = 1;
    driftData.speed = speed;
    driftData.steering = steering;
    driftData.music = music;

    if (selfID == 0) {
        esp_now_send(adress1.data(), (uint8_t *) &driftData, sizeof(driftData));
    } else if (selfID == 1) {
        Serial.print("Music: ");
        Serial.println(driftData.music);
        esp_now_send(adress0.data(), (uint8_t *) &driftData, sizeof(driftData));
    } else {Serial.println("Something fucked");}
}

void OnDataSent(const uint8_t *macAdr, esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS) {
        Serial.println("Sent data not succ");
    } else {
        // Serial.println("Sent data succ.");
    }
}

void adjustMotors(int speed, int steering) {
    // Clamp inputs to safe PWM range
    speed = constrain(speed, -255, 255);
    steering = constrain(steering, -255, 255);

    // Calculate individual motor speeds
    int leftSpeed = speed + steering;   // turning right increases left motor speed
    int rightSpeed = speed - steering;  // turning left increases right motor speed

    // Clamp to PWM range
    leftSpeed = constrain(leftSpeed, -255, 255);
    rightSpeed = constrain(rightSpeed, -255, 255);

    // Apply to motors
    motor->turnLeft(leftSpeed);
    motor->turnRight(rightSpeed);
}

unsigned long sendTime;
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&recievingData, incomingData, sizeof(recievingData));
    // Serial.println("Recieving data!");

    if (selfID == 0) {
        // Car
        adjustMotors(recievingData.speed, recievingData.steering);
        sendData(recievingData.speed, recievingData.steering, recievingData.music);

        buzzer->remoteControlMusic = static_cast<bool>(driftData.music);
        if (!driftData.music) {
            Serial.println("No music :c");
        } else {
            Serial.println("Music!");
        }

    } else if (selfID == 1) {
        auto recTime = millis();
        // Serial.print("We got back data! Time: ");
        // Serial.print(recTime - sendTime);
        // Serial.println(" ms.");

        Serial.print("Got back Music: ");
        Serial.println(recievingData.music);

        if (recievingData.speed != driftData.speed or recievingData.steering != driftData.steering) {
            Serial.println("Got wrong data...");
        } // else {// Serial.println("Got correct data :D");}
    }
}

void printAdr(uint8_t *adr) {
    for (int i = 0; i < 5; i++) {
        Serial.print(adr[i]);
        Serial.print(":");
    }
    Serial.println(adr[5]);
}

unsigned long startTime;
uint8_t baseMac[6];
void setup() {

    Serial.begin(115200);
    Serial.println("Serial configured.");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        while (true) {
            Serial.println("ESP_Now does not work or whatever");
            delay(1000);
        }
    }

    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);

    if (baseMac[5] == 156) {selfID = 0;} else {selfID = 1;}

    Serial.println("Adress: ");
    printAdr(baseMac);
    Serial.print("Unit: ");
    if (selfID == 0) {
        Serial.println("Car.");
    } else if (selfID == 1) {
        Serial.println("Controller.");
    } else {Serial.println("Something fucked lol.");}


    Serial.println("ESP_Now configured.");
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (selfID == 0) {
        memcpy(peerInfo.peer_addr, adress1.data(), 6);
    } else {
        memcpy(peerInfo.peer_addr, adress0.data(), 6);
    }

    auto val = esp_now_add_peer(&peerInfo);
    if (val != ESP_OK) {
        Serial.print("Failed to register ESP for espNOW");
    }

    startTime = millis();

    if (selfID == 1) {
        encoderSpeed = new RotaryEncoder{21, 19, 18};
        encoderSteering = new RotaryEncoder(16, 17, 25);
        pinMode(13, INPUT_PULLDOWN);
    } else {
        motor = new Motor(14, 27, 26, 25);
        buzzer = new Buzzer(33);
    }
}

void loop() {
    if (selfID == 1) {
        encoderSpeed->readRotary();
        encoderSteering->readRotary();
    }

    bool shouldSendData = (selfID == 1) && (millis() - startTime > 100);
    if (shouldSendData) {
        sendTime = millis();
        sendData(encoderSpeed->value * 5, encoderSteering->value, digitalRead(13));

        Serial.print("Speed: ");
        Serial.print(encoderSpeed->value * 5);
        Serial.print(". Steering: ");
        Serial.println(encoderSteering->value);

        startTime = millis();
    }

    if (selfID == 0) {
        buzzer->update();

        if (millis() - buzzer->noteStart > 5000) {
            buzzer->start();
        }
    }
}