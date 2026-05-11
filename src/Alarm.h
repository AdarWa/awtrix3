//
// Created by adarw on 5/11/26.
//

#ifndef AWTRIX3_ALARM_H
#define AWTRIX3_ALARM_H
#include "fl/optional.h"
#include "time.h"

class Alarm {
private:
    Alarm() = default;
    fl::optional<time_t> nextAlarm = fl::nullopt;
public:
    Alarm(const Alarm&) = delete;
    void operator=(const Alarm&) = delete;

    static Alarm& getInstance();
    void setup();
    void loop();
    void setAlarm(time_t time);
    fl::optional<time_t> fetchAlarm();
    fl::optional<time_t> getNextAlarm();
};


#endif //AWTRIX3_ALARM_H