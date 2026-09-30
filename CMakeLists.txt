cmake_minimum_required(VERSION 3.21)

# Тулчейн vcpkg должен подключаться ДО первого project().
# Передаётся либо пресетом (CMakePresets.json), либо вручную:
#   -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake

project(solar-system
    VERSION 0.1.0
    DESCRIPTION "Interactive C++17 solar system visualization"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
option(SOLAR_BUILD_APP "Build the graphical application" ON)
if(BUILD_TESTING)
    add_executable(localization-test tests/localization_test.cpp)
    target_include_directories(localization-test PRIVATE include)
    if(MSVC)
        target_compile_options(localization-test PRIVATE /utf-8)
    endif()
    add_test(NAME localization COMMAND localization-test)
    add_executable(sphere-mapping-test tests/sphere_mapping_test.cpp)
    target_include_directories(sphere-mapping-test PRIVATE include)
    add_test(NAME sphere-mapping COMMAND sphere-mapping-test)
    add_executable(physical-scale-test tests/physical_scale_test.cpp)
    target_include_directories(physical-scale-test PRIVATE include)
    add_test(NAME physical-scale COMMAND physical-scale-test)
endif()
if(NOT SOLAR_BUILD_APP)
    return()
endif()

# Единый каталог для исполняемого файла (чтобы рядом лежали assets/).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Зависимости (через vcpkg) -------------------------------------------
if(EMSCRIPTEN)
    include(FetchContent)
    set(PLATFORM Web CACHE STRING "raylib platform" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "raylib examples" FORCE)
    set(CUSTOMIZE_BUILD OFF CACHE BOOL "Use raylib's default configuration" FORCE)
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG dbc56a87da87d973a9c5baa4e7438a9d20121d28)
    FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG 8936b58fe26e8c3da834b8f60b06511d537b4c63)
    FetchContent_MakeAvailable(raylib imgui)
    # JPEG is disabled in raylib 6's default web build; enable just this format.
    target_compile_definitions(raylib PRIVATE SUPPORT_FILEFORMAT_JPG=1)
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
    add_library(imgui::imgui ALIAS imgui)
else()
find_package(raylib CONFIG REQUIRED)
# raylib в vcpkg собирается с внешним glfw3, но не пробрасывает эту
# зависимость в своих CMake-таргетах — линкуем её явно.
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
endif()

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
if(NOT EMSCRIPTEN AND LIBNOVA_INCLUDE_DIR AND LIBNOVA_LIBRARY)
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
if(EMSCRIPTEN)
    target_compile_definitions(solar-system PRIVATE ASSETS_DIR="/assets")
else()
    target_compile_definitions(solar-system PRIVATE ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets")
endif()

target_link_libraries(solar-system PRIVATE
    rlImGui              # транзитивно тянет raylib и imgui::imgui (PUBLIC)
)
if(EMSCRIPTEN)
    set_target_properties(solar-system PROPERTIES
        OUTPUT_NAME index SUFFIX ".html"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/web")
    # raylib's synchronous web loop yields through emscripten_sleep;
    # Asyncify keeps the existing desktop loop and RAII ownership intact.
    target_link_options(solar-system PRIVATE
        "-sASYNCIFY=1" "-sALLOW_MEMORY_GROWTH=1" "-sSTACK_SIZE=1048576"
        "-sENVIRONMENT=web"
        "--preload-file" "${CMAKE_SOURCE_DIR}/assets@/assets"
        "--shell-file" "${CMAKE_SOURCE_DIR}/web/shell.html")
    set_property(TARGET solar-system APPEND PROPERTY LINK_DEPENDS
        "${CMAKE_SOURCE_DIR}/web/shell.html"
        "${CMAKE_SOURCE_DIR}/assets/shaders/lighting-web.vs"
        "${CMAKE_SOURCE_DIR}/assets/shaders/lighting-web.fs")
    file(GLOB_RECURSE SOLAR_ASSET_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/assets/*")
    set_property(TARGET solar-system APPEND PROPERTY LINK_DEPENDS ${SOLAR_ASSET_FILES})
else()
    target_link_libraries(solar-system PRIVATE glfw)
endif()

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
    target_compile_options(solar-system PRIVATE /W4 /utf-8)
    # std::getenv и т.п. помечены в MSVC как «небезопасные» (C4996) — глушим,
    # код переносимый и стандартный.
    target_compile_definitions(solar-system PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(solar-system PRIVATE -Wall -Wextra)
endif()
