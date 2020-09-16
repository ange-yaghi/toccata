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
    m_threshold.SetBoundingBox(m_boundingBox);
    m_threshold.SetMin(0.0);
    m_threshold.SetMax(1.0);
    m_threshold.SetStep(0.002);
    m_threshold.SetPrecision(4);
    m_threshold.SetTextHeight((double)m_boundingBox.Height() * 0.9);
    m_threshold.SetWrap(true);
}
