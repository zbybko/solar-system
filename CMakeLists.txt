cmake_minimum_required(VERSION 3.21)

# Тулчейн vcpkg должен подключаться ДО первого project().
# Передаётся либо пресетом (CMakePresets.json), либо вручную:
#   -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake

project(solar-system
    VERSION 0.1.0
    DESCRIPTION "Объектная 3D-модель Солнечной системы"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Единый каталог для исполняемого файла (чтобы рядом лежали assets/).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Зависимости (через vcpkg) -------------------------------------------
find_package(raylib CONFIG REQUIRED)
# raylib в vcpkg собирается с внешним glfw3, но не пробрасывает эту
# зависимость в своих CMake-таргетах — линкуем её явно.
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# libnova — ОПЦИОНАЛЬНЫЙ провайдер эфемерид. В реестре vcpkg её нет; если она
# установлена системно (macOS — 'brew install libnova', Debian/Ubuntu —
# 'sudo apt install libnova-dev'), подключаем провайдер LibnovaEphemeris.
# Если её нет (типично для Windows) — проект собирается без неё и использует
# собственный KeplerEphemeris (без внешних зависимостей).
find_path(LIBNOVA_INCLUDE_DIR
    NAMES libnova/ln_types.h
    HINTS /opt/homebrew/include /usr/local/include /usr/include)
find_library(LIBNOVA_LIBRARY
    NAMES nova libnova
    HINTS /opt/homebrew/lib /usr/local/lib /usr/lib)
if(LIBNOVA_INCLUDE_DIR AND LIBNOVA_LIBRARY)
    set(HAVE_LIBNOVA TRUE)
    message(STATUS "libnova найдена: ${LIBNOVA_LIBRARY} — провайдер LibnovaEphemeris включён")
else()
    set(HAVE_LIBNOVA FALSE)
    message(STATUS "libnova не найдена — используется KeplerEphemeris (без внешних зависимостей)")
endif()

# --- rlImGui (вендорный мост Dear ImGui ↔ raylib) ------------------------
# В реестре vcpkg rlImGui нет, поэтому исходники лежат в third_party/.
# Собираем отдельной библиотекой без -Wall -Wextra (чужой код) и с
# NO_FONT_AWESOME, чтобы не тащить встроенные иконки FontAwesome.
add_library(rlImGui STATIC third_party/rlImGui/rlImGui.cpp)
target_include_directories(rlImGui PUBLIC ${CMAKE_SOURCE_DIR}/third_party/rlImGui)
target_compile_definitions(rlImGui PUBLIC NO_FONT_AWESOME RLIMGUI_STATIC)
target_link_libraries(rlImGui PUBLIC raylib imgui::imgui)

# --- Цель ----------------------------------------------------------------
add_executable(solar-system
    src/main.cpp
    src/ephemeris/KeplerEphemeris.cpp
    src/UIManager.cpp
    src/CelestialBody.cpp
    src/Star.cpp
    src/Planet.cpp
    src/Moon.cpp
    src/SolarSystem.cpp
    src/SimulationClock.cpp
    src/CameraController.cpp
    src/Renderer.cpp
    src/Starfield.cpp
)

target_include_directories(solar-system PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Путь к ассетам известен на этапе компиляции — приложение находит шрифты и
# текстуры независимо от текущего рабочего каталога при запуске.
target_compile_definitions(solar-system PRIVATE
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets")

target_link_libraries(solar-system PRIVATE
    rlImGui              # транзитивно тянет raylib и imgui::imgui (PUBLIC)
    glfw                 # raylib-конфиг vcpkg не пробрасывает glfw — линкуем явно
)

# Подключаем libnova-провайдер только если библиотека найдена (см. выше).
if(HAVE_LIBNOVA)
    target_sources(solar-system PRIVATE src/ephemeris/LibnovaEphemeris.cpp)
    target_compile_definitions(solar-system PRIVATE HAVE_LIBNOVA)
    target_include_directories(solar-system PRIVATE ${LIBNOVA_INCLUDE_DIR})
    target_link_libraries(solar-system PRIVATE ${LIBNOVA_LIBRARY})
endif()

# raylib на macOS требует системные фреймворки.
if(APPLE)
    target_link_libraries(solar-system PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
endif()

# --- Предупреждения ------------------------------------------------------
if(MSVC)
    target_compile_options(solar-system PRIVATE /W4)
    # std::getenv и т.п. помечены в MSVC как «небезопасные» (C4996) — глушим,
    # код переносимый и стандартный.
    target_compile_definitions(solar-system PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(solar-system PRIVATE -Wall -Wextra)
endif()
