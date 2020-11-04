#include "../include/bounding_box.h"

toccata::BoundingBox::BoundingBox() {
    m_min = { FLT_MAX, FLT_MAX };
    m_max = { -FLT_MAX, -FLT_MAX };
}

toccata::BoundingBox::BoundingBox(float width, float height) {
    m_min = { 0.0f, 0.0f };
    m_max = { width, height };
}

toccata::BoundingBox::~BoundingBox() {
    /* void */
}

float toccata::BoundingBox::Width() const {
    return m_max.x - m_min.x;
}

float toccata::BoundingBox::Height() const {
    return m_max.y - m_min.y;
}

float toccata::BoundingBox::CenterX() const {
    return (m_min.x + m_max.x) / 2.0f;
}

float toccata::BoundingBox::CenterY() const {
    return (m_min.y + m_max.y) / 2.0f;
}

ysVector2 toccata::BoundingBox::GetCenter() const {
    return { CenterX(), CenterY() };
}

toccata::BoundingBox toccata::BoundingBox::MarginOffset(float dx, float dy) const {
    return BoundingBox(Width() + dx * 2, Height() + dy * 2)
        .AlignCenterX(CenterX())
        .AlignCenterY(CenterY());
}

toccata::BoundingBox &toccata::BoundingBox::AlignTop(float top) {
    const float dy = Top() - top;
    m_min.y -= dy;
    m_max.y -= dy;

    return *this;
}

toccata::BoundingBox &toccata::BoundingBox::AlignBottom(float bottom) {
    const float dy = Bottom() - bottom;
    m_min.y -= dy;
    m_max.y -= dy;

    return *this;
}

toccata::BoundingBox &toccata::BoundingBox::AlignRight(float right) {
    const float dx = Right() - right;
    m_min.x -= dx;
    m_max.x -= dx;

    return *this;
}

toccata::BoundingBox &toccata::BoundingBox::AlignLeft(float left) {
    const float dx = Left() - left;
    m_min.x -= dx;
    m_max.x -= dx;

    return *this;
}

toccata::BoundingBox &toccata::BoundingBox::AlignCenterX(float hCenter) {
    const float dx = CenterX() - hCenter;
    m_min.x -= dx;
    m_max.x -= dx;

    return *this;
}

toccata::BoundingBox &toccata::BoundingBox::AlignCenterY(float vCenter) {
    const float dy = CenterY() - vCenter;
    m_min.y -= dy;
    m_max.y -= dy;

    return *this;
}

toccata::BoundingBox toccata::BoundingBox::Add(const BoundingBox &box) const {
    BoundingBox result;
    result.m_min.x = min(box.m_min.x, m_min.x);
    result.m_max.x = max(box.m_max.x, m_max.x);
    result.m_min.y = min(box.m_min.y, m_min.y);
    result.m_max.y = max(box.m_max.y, m_max.y);

    return result;
}
