//
// Created by adarw on 5/11/26.
//

#include "Alarm.h"

#include <cstdio>
#include <LittleFS.h>

#include "DisplayManager.h"
#include "timer.h"

static constexpr auto ALARM_PATH = "/alarm.txt";
static constexpr auto ALARM_NOTIFICATION_PATH = "/alarm.json";

static void storeAlarmTime(time_t time) {
    char buff[20] = {};
    int len = snprintf(buff, sizeof(buff), "%lld", static_cast<long long>(time));

    File f = LittleFS.open(ALARM_PATH, "w", true);
    if (f) {
        f.write(reinterpret_cast<uint8_t*>(buff), len);
        f.close();
    }
}

Alarm& Alarm::getInstance() {
    static Alarm inst;
    return inst;
}

void Alarm::setup() {
    nextAlarm = fl::nullopt;
    fetchAlarm();
}

static void generateAlarm() {
    if (LittleFS.exists(ALARM_NOTIFICATION_PATH)) {
        File f = LittleFS.open(ALARM_NOTIFICATION_PATH, "r");
        if (f) {
            size_t size = f.size();
            char* buff = new char[size + 1];

            f.readBytes(buff, size);
            buff[size] = '\0';
            f.close();

            DisplayManager_::getInstance().generateNotification(1, buff);

            delete[] buff;
        }
    }else {
        Serial.println("[Alarm] Could not generate alarm notification! Notification definition file doesn't exists.");
    }
}

void Alarm::loop() {
    if (!nextAlarm) {
        return;
    }
    time_t time = timer_time();
    if (time > *nextAlarm) {
        Serial.println(*nextAlarm);
        generateAlarm();
        nextAlarm = fl::nullopt;
    }
}

void Alarm::setAlarm(time_t time) {
    nextAlarm = time;
    storeAlarmTime(time);
}

fl::optional<time_t> Alarm::fetchAlarm() {
    if (LittleFS.exists(ALARM_PATH)) {
        File f = LittleFS.open(ALARM_PATH, "r");

        if (f && f.size() > 0) {
            char buff[20] = {};

            size_t bytesRead = f.readBytes(buff, sizeof(buff) - 1);
            buff[bytesRead] = '\0';
            f.close();

            char* endPtr;
            time_t storedTime = strtoll(buff, &endPtr, 10);
            if (storedTime < timer_time()) {
                nextAlarm = fl::nullopt;
                return fl::nullopt;
            }

            nextAlarm = fl::optional<time_t>(storedTime);
            return nextAlarm;
        }
    }

    return fl::nullopt;
}

fl::optional<time_t> Alarm::getNextAlarm() {
    return nextAlarm;
}


