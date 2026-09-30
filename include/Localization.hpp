#pragma once
#include <array>
#include <cstddef>
#include <string_view>

namespace solar {
enum class Language { English, German, Russian };
enum class BodyKind { Star, Planet, Moon };
enum class Text {
    AppTitle, Controls, LanguageLabel, Provider, Kepler, Camera, Orbital, Free,
    SwitchFree, SwitchOrbit, ResetView, Instructions, Display, Compact, Real,
    Orbits, Grid, Ambient, Time, Pause, Speed, ResetTime, BodyInfo, Type,
    Radius, Tilt, Rotation, Position, Distance,
    Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Moon,
    StarType, PlanetType, MoonType, SelectionWireframe, SelectBody, RealScaleHint, Count
};
// Translated presentation strings; stable IDs preserve ImGui state across languages.
inline constexpr std::array<std::array<const char*, 3>, static_cast<std::size_t>(Text::Count)> translations{{
    {"Solar System", "Sonnensystem", "Солнечная система"},
    {"Controls###controls", "Steuerung###controls", "Панель управления###controls"},
    {"Language", "Sprache", "Язык"},
    {"Ephemeris provider", "Ephemeridenmodell", "Провайдер эфемерид"},
    {"Kepler (JPL elements)", "Kepler (JPL-Elemente)", "Kepler (элементы JPL)"},
    {"Camera", "Kamera", "Камера"},
    {"Orbital", "Orbit", "Орбита"},
    {"Free flight", "Freier Flug", "Свободный полёт"},
    {"Free flight###cameraMode", "Freier Flug###cameraMode", "В свободный полёт###cameraMode"},
    {"Orbit mode###cameraMode", "Orbitmodus###cameraMode", "В режим орбиты###cameraMode"},
    {"Reset view###resetView", "Ansicht zurücksetzen###resetView", "Сбросить вид###resetView"},
    {"Left click: select | Right drag: rotate | Wheel: zoom/speed | F: camera mode | WASD + Q/E: fly",
     "Linksklick: auswählen | Rechts ziehen: drehen | Mausrad: Zoom/Tempo | F: Kameramodus | WASD + Q/E: fliegen",
     "ЛКМ: выбрать | ПКМ: поворот | Колесо: зум/скорость | F: режим камеры | WASD + Q/E: полёт"},
    {"Display", "Darstellung", "Отображение"},
    {"Compact###compact", "Kompakt###compact", "Компактный###compact"},
    {"Real distances###real", "Reale Abstände###real", "Реальные расстояния###real"},
    {"Orbits###orbits", "Umlaufbahnen###orbits", "Орбиты###orbits"},
    {"Grid###grid", "Raster###grid", "Сетка###grid"},
    {"Ambient light###ambient", "Umgebungslicht###ambient", "Фоновый свет###ambient"},
    {"Time", "Zeit", "Время"},
    {"Pause###pause", "Pause###pause", "Пауза###pause"},
    {"Days / second###speed", "Tage / Sekunde###speed", "Сутки / секунду###speed"},
    {"Reset time###resetTime", "Zeit zurücksetzen###resetTime", "Сбросить время###resetTime"},
    {"Body information###bodyInfo", "Himmelskörper###bodyInfo", "Информация о теле###bodyInfo"},
    {"Type", "Typ", "Тип"},
    {"Mean radius (km)", "Mittlerer Radius (km)", "Средний радиус (км)"},
    {"Axial tilt", "Achsenneigung", "Наклон оси"},
    {"Rotation period (h)", "Rotationsdauer (h)", "Период вращения (ч)"},
    {"Heliocentric position (AU)", "Heliozentrische Position (AE)", "Гелиоцентрическая позиция (а.е.)"},
    {"Distance to Sun (AU)", "Abstand zur Sonne (AE)", "Расстояние до Солнца (а.е.)"},
    {"Sun", "Sonne", "Солнце"}, {"Mercury", "Merkur", "Меркурий"},
    {"Venus", "Venus", "Венера"}, {"Earth", "Erde", "Земля"},
    {"Mars", "Mars", "Марс"}, {"Jupiter", "Jupiter", "Юпитер"},
    {"Saturn", "Saturn", "Сатурн"}, {"Uranus", "Uranus", "Уран"},
    {"Neptune", "Neptun", "Нептун"}, {"Moon", "Mond", "Луна"},
    {"Star", "Stern", "Звезда"}, {"Planet", "Planet", "Планета"},
    {"Natural satellite", "Natürlicher Satellit", "Спутник"},
    {"Selection wireframe###selectionWireframe", "Auswahlgitter###selectionWireframe", "Сетка выделения###selectionWireframe"},
    {"Select body###selectBody", "Himmelskörper wählen###selectBody", "Выбрать тело###selectBody"},
    {"True sizes and distances. Use the body selector to inspect tiny planets.", "Reale Größen und Abstände. Kleine Planeten über die Auswahl betrachten.", "Реальные размеры и расстояния. Используйте список для приближения к планетам."}
}};
inline const char* text(Text key, Language language) {
    return translations.at(static_cast<std::size_t>(key)).at(static_cast<std::size_t>(language));
}
inline Language languageFromCode(std::string_view code) {
    if (code == "de") return Language::German;
    if (code == "ru") return Language::Russian;
    return Language::English;
}
} // namespace solar
