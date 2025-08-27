//
// Created by Øystein Bringsli.
//

#ifndef CONTROLLER_BUZZER_HPP
#define CONTROLLER_BUZZER_HPP

#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880

int melody[] = {
        NOTE_D4, NOTE_D4, NOTE_D4, NOTE_F4, NOTE_G4,
        NOTE_D4, NOTE_D4, NOTE_D4, NOTE_F4, NOTE_G4,
        NOTE_D4, NOTE_D4, NOTE_D4, NOTE_F4, NOTE_G4,
        NOTE_A4, NOTE_G4, NOTE_F4, NOTE_D4
};

// Note durations (4 = quarter, 8 = eighth)
int noteDurations[] = {
        8, 8, 8, 8, 4,
        8, 8, 8, 8, 4,
        8, 8, 8, 8, 4,
        8, 8, 8, 2
};

class Buzzer {
private:
    const int channel = 4;
    const int freq = 5000;
    const int resolution = 8;

    unsigned long previousMillis = 0;
    int currentNote = 0;
    bool playing = false;

    int pin;
public:
    Buzzer(int pin);
    void start();
    void update();

    unsigned long noteStart = 0;
    bool remoteControlMusic = false;
};

Buzzer::Buzzer(int pin): pin(pin) {
    ledcSetup(channel, freq, resolution);
    ledcAttachPin(pin, channel);
}

void Buzzer::start() {
    if (remoteControlMusic) {
        playing = true;
        noteStart = millis();
        currentNote = 0;
    }
}

void Buzzer::update() {
    if (playing && remoteControlMusic) {
        unsigned long now = millis();

        int noteDurationMs = 1000 / noteDurations[currentNote];
        // Check if it's time to move to the next note
        if (now - noteStart >= noteDurationMs) {
            ledcWrite(channel, 0);
            currentNote++;

            if (currentNote >= sizeof(melody) / sizeof(melody[0])) {
                playing = false; // Finished
            } else {
                noteStart = now;
                if (melody[currentNote] > 0) {
                    ledcWriteTone(channel, melody[currentNote]);
                }
            }
        }
    } else {
        ledcWrite(channel, 0);
    }
}


#endif //CONTROLLER_BUZZER_HPP
