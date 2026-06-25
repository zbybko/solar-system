#pragma once

// KeplerEphemeris — альтернативный провайдер эфемерид без внешних библиотек.
//
// ООП-приём: ВТОРАЯ КОНКРЕТНАЯ СТРАТЕГИЯ для IEphemeris. Считает позиции планет
// по классическим кеплеровым элементам орбит (таблица JPL на эпоху J2000 с
// вековыми скоростями) — решает уравнение Кеплера и переводит результат в те же
// гелиоэклиптические координаты, что и LibnovaEphemeris. Нужен, чтобы показать
// взаимозаменяемость провайдеров (переключение из UI) и работоспособность без
// libnova. По умолчанию приложение использует libnova; этот провайдер — запасной.

#include "ephemeris/IEphemeris.hpp"

namespace solar {

class KeplerEphemeris final : public IEphemeris {
public:
    Vec3 heliocentricPosition(BodyId body, double julianDay) const override;
    Vec3 moonGeocentricPosition(double julianDay) const override;
    const char* providerName() const override { return "Kepler (элементы JPL)"; }
};

} // namespace solar
