#pragma once

// SimulationClock — модельное время симуляции.
//
// ООП-приём: ИНКАПСУЛЯЦИЯ (единственный ответственный за юлианскую дату и
// множитель времени; инварианты — скорость не отрицательна, пауза не теряет
// дату). Реальное время кадра превращается в ход модельных суток с заданной
// скоростью; поддерживает паузу и сброс к эпохе старта.

namespace solar {

class SimulationClock {
public:
    // По умолчанию эпоха — текущий момент (UTC); скорость в сутках/сек.
    explicit SimulationClock(double speedDaysPerSecond = 5.0);

    // Продвинуть время на realDeltaSeconds реального времени.
    void advance(double realDeltaSeconds);

    void reset();                 // вернуться к эпохе старта
    void setPaused(bool paused) { paused_ = paused; }
    bool isPaused() const { return paused_; }

    void setSpeed(double daysPerSecond);
    double speed() const { return speed_; }

    double julianDay() const { return epochJd_ + elapsedDays_; }
    double elapsedDays() const { return elapsedDays_; }

    // Юлианская дата для текущего момента UTC (используется как эпоха).
    static double julianDayUtcNow();

private:
    double epochJd_;        // юлианская дата старта
    double elapsedDays_{0.0};
    double speed_;          // модельных суток за секунду реального времени
    bool paused_{false};
};

} // namespace solar
