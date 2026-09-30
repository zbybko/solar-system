#include "Localization.hpp"
#include <iostream>
#include <string_view>

int main() {
    using namespace solar;
    int failures = 0;
    auto check = [&](bool condition, const char* message) {
        if (!condition) { std::cerr << message << '\n'; ++failures; }
    };
    for (std::size_t i = 0; i < static_cast<std::size_t>(Text::Count); ++i) {
        std::string_view referenceId;
        for (Language language : {Language::English, Language::German, Language::Russian}) {
            const char* value = text(static_cast<Text>(i), language);
            check(value && *value, "Missing translation");
            if (!value) continue;
            const std::string_view label(value);
            const auto marker = label.find("###");
            const auto id = marker == std::string_view::npos ? std::string_view{} : label.substr(marker);
            if (language == Language::English) referenceId = id;
            else check(id == referenceId, "Translated widget ID changed");
        }
    }
    check(languageFromCode("de") == Language::German, "German language parsing");
    check(languageFromCode("ru") == Language::Russian, "Russian language parsing");
    check(languageFromCode("en") == Language::English, "English language parsing");
    check(languageFromCode("unknown") == Language::English, "Unknown language fallback");
    check(languageFromCode("") == Language::English, "Empty language fallback");
    check(std::string_view(text(Text::Earth, Language::German)) == "Erde", "German body name");
    check(std::string_view(text(Text::Moon, Language::English)) == "Moon", "English moon name");
    check(std::string_view(text(Text::MoonType, Language::German)) == "Natürlicher Satellit", "German body type");
    if (failures) return 1;
    std::cout << "All localization entries and stable UI IDs verified in EN/DE/RU.\n";
}
