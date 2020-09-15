#include "../include/timing_practice_controls.h"

toccata::TimingPracticeControls::TimingPracticeControls() {
    /* void */
}

toccata::TimingPracticeControls::~TimingPracticeControls() {
    /* void */
}

void toccata::TimingPracticeControls::Construct() {
    AddChild(&m_threshold);
}

void toccata::TimingPracticeControls::Render() {
    Component::Render();
}

void toccata::TimingPracticeControls::Update() {
    m_threshold.SetPosition(m_position);
    m_threshold.SetSize(m_size);
    m_threshold.SetMin(0.0);
    m_threshold.SetMax(1.0);
    m_threshold.SetStep(0.002);
    m_threshold.SetPrecision(4);
    m_threshold.SetTextHeight(m_size.y * 0.9f);
    m_threshold.SetWrap(true);
}
