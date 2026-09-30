#pragma once

// assetPath — единая точка получения пути к файлу ассета.
//
// Для перенесённых сборок (скачанный архив) ассеты лежат рядом с исполняемым
// файлом — ищем их там в первую очередь. Если не нашли (локальная разработка
// из дерева исходников) — берём путь из ASSETS_DIR, заданного при сборке.

#include "raylib.h"

#include <string>

namespace solar {

inline std::string assetPath(const std::string& relative) {
#ifdef __EMSCRIPTEN__
    return std::string(ASSETS_DIR) + "/" + relative;
#else
    const std::string nearExe = std::string(GetApplicationDirectory()) + "assets/" + relative;
    if (FileExists(nearExe.c_str()))
        return nearExe;
    return std::string(ASSETS_DIR) + "/" + relative;
#endif
}

} // namespace solar
