#include "SimulationClock.hpp"

#include <ctime>

namespace solar {

SimulationClock::SimulationClock(double speedDaysPerSecond)
    : epochJd_(julianDayUtcNow()),
      speed_(speedDaysPerSecond < 0.0 ? 0.0 : speedDaysPerSecond) {}

void SimulationClock::advance(double realDeltaSeconds) {
    if (paused_)
        return;
    elapsedDays_ += speed_ * realDeltaSeconds;
}

void SimulationClock::reset() {
    elapsedDays_ = 0.0;
}

void SimulationClock::setSpeed(double daysPerSecond) {
    speed_ = daysPerSecond < 0.0 ? 0.0 : daysPerSecond;
}

double SimulationClock::julianDayUtcNow() {
    std::time_t t = std::time(nullptr);
    std::tm utc{};
#if defined(_WIN32)
    gmtime_s(&utc, &t);
#else
    gmtime_r(&t, &utc);
#endif
    const int year = utc.tm_year + 1900;
    const int month = utc.tm_mon + 1;
    const int day = utc.tm_mday;
    const int a = (14 - month) / 12;
    const int y = year + 4800 - a;
    const int m = month + 12 * a - 3;
    const long jdn =
        day + (153 * m + 2) / 5 + 365L * y + y / 4 - y / 100 + y / 400 - 32045;
    const double frac =
        (utc.tm_hour - 12) / 24.0 + utc.tm_min / 1440.0 + utc.tm_sec / 86400.0;
    return static_cast<double>(jdn) + frac;
}

} // namespace solar
