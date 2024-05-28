//
// Created by davidl09 on 5/28/24.
//

#ifndef CAM_TIMESTR_H
#define CAM_TIMESTR_H

#include <string>
#include <chrono>

using namespace std::chrono;

std::string  getCurrentTimeStr() {
    const auto now = system_clock::to_time_t(system_clock::now());
    std::tm now_tm = *std::localtime(&now);
    return std::format("{:04}-{:02}-{:02}_{:02}:{:02}:{:02}",
                       now_tm.tm_year + 1900,
                       now_tm.tm_mon + 1,
                       now_tm.tm_mday,
                       now_tm.tm_hour,
                       now_tm.tm_min,
                       now_tm.tm_sec);
}

#endif //CAM_TIMESTR_H
