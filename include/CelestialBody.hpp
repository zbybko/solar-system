#pragma once

// CelestialBody — абстрактный базовый класс всех небесных тел.
//
// ООП-приёмы: АБСТРАКЦИЯ (нельзя создать «просто тело»), ПОЛИМОРФИЗМ
// (update/draw — виртуальные, клиент работает через CelestialBody*),
// ИНКАПСУЛЯЦИЯ (поля защищены, доступ через геттеры), RAII (тело владеет
// своей Model/Texture2D и выгружает их в деструкторе). Виртуальный деструктор
// обязателен: тела хранятся и удаляются через unique_ptr<CelestialBody>.

#include "raylib.h"
#include "RenderContext.hpp"
#include "ephemeris/IEphemeris.hpp"

#include <string>

namespace solar {

class CelestialBody {
public:
    CelestialBody(std::string name, BodyId id, float radius,
                  float axialTiltDeg, float rotationPeriodHours, Color color);
    virtual ~CelestialBody();

    // Тело владеет GPU-ресурсами — копирование запрещено (RAII).
    CelestialBody(const CelestialBody&) = delete;
    CelestialBody& operator=(const CelestialBody&) = delete;

    // Пересчитать состояние (позицию, угол суточного вращения) на дату jd.
    virtual void update(double jd, const IEphemeris& ephemeris) = 0;

    // Нарисовать тело с учётом масштаба из контекста.
    virtual void draw(const RenderContext& ctx) const = 0;

    // Нарисовать орбиту тела (по умолчанию её нет — переопределяет Planet).
    virtual void drawOrbit(const RenderContext& /*ctx*/) const {}

    // Пересчитать орбиту под новый провайдер (по умолчанию — ничего).
    virtual void rebuildOrbit(const IEphemeris& /*ephemeris*/) {}

    // Тип тела для UI («Звезда» / «Планета» / «Спутник»).
    virtual const char* typeName() const = 0;

    // Освещается ли тело внешним источником. Звезда сама светит — false.
    virtual bool isLit() const { return true; }

    // Назначить шейдер материалу модели (для освещения от Солнца).
    void setShader(Shader shader) { model_.materials[0].shader = shader; }

    // Гелиоцентрическая позиция в а.е. (для камеры, выбора, орбит).
    Vector3 worldPosition() const { return worldPos_; }

    // Позиция тела в мировых координатах raylib (с учётом масштаба контекста) —
    // нужна камере и подбору тела лучом.
    Vector3 renderPosition(const RenderContext& ctx) const { return scaledPosition(ctx); }
    const std::string& name() const { return name_; }
    BodyId id() const { return id_; }
    float radius() const { return radius_; }
    float axialTilt() const { return axialTilt_; }
    float rotationPeriodHours() const { return rotationPeriod_; }

protected:
    // Загрузить текстуру по пути; при отсутствии файла остаётся цветная сфера.
    void loadTexture(const char* path);

    // Обновить угол суточного вращения для даты jd.
    void updateRotation(double jd);

    // Общая отрисовка сферы: перенос в позицию, наклон оси, суточное вращение.
    void renderSphere(const RenderContext& ctx) const;

    // Позиция тела в мировых координатах raylib с учётом масштаба контекста.
    Vector3 scaledPosition(const RenderContext& ctx) const;

    std::string name_;
    BodyId id_;
    float radius_;             // отображаемый радиус, мировые единицы
    float axialTilt_;          // наклон оси, градусы
    float rotationPeriod_;     // период вращения, часы (<=0 — не вращается)
    float rotationAngle_{0.f}; // текущий угол вокруг оси, градусы
    Color color_;
    Vector3 worldPos_{0.f, 0.f, 0.f}; // гелиоцентрические, а.е.

    Model model_{};
    Texture2D texture_{};
    bool hasTexture_{false};
};

} // namespace solar
