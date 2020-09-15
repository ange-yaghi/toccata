#include "../include/velocity_practice_controls.h"

toccata::VelocityPracticeControls::VelocityPracticeControls() {
    /* void */
}

toccata::VelocityPracticeControls::~VelocityPracticeControls() {
    /* void */
}

void toccata::VelocityPracticeControls::Construct() {
    AddChild(&m_threshold);
    AddChild(&m_target);
}

void toccata::VelocityPracticeControls::Render() {
    Component::Render();
}

void toccata::VelocityPracticeControls::Update() {
    m_target.SetPosition(m_position);
    m_target.SetSize({ m_size.x / 2, m_size.y });
    m_target.SetMin(0.0);
    m_target.SetMax(128.0);
    m_target.SetStep(0.5);
    m_target.SetPrecision(4);
    m_target.SetTextHeight(m_size.y * 0.9f);
    m_target.SetWrap(true);

    m_threshold.SetPosition({ m_position.x + m_size.x / 2, m_position.y});
    m_threshold.SetSize({ m_size.x / 2, m_size.y });
    m_threshold.SetMin(0.0);
    m_threshold.SetMax(128.0);
    m_threshold.SetStep(0.5);
    m_threshold.SetPrecision(4);
    m_threshold.SetTextHeight(m_size.y * 0.9f);
    m_threshold.SetWrap(true);
}
