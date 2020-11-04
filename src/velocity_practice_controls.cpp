#include "../include/velocity_practice_controls.h"

#include "../include/grid.h"

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
    Grid grid(m_boundingBox, 2, 1, 5.0f);

    m_target.SetBoundingBox(grid.GetCell(0, 0));
    m_target.SetMin(0.0);
    m_target.SetMax(128.0);
    m_target.SetStep(0.5);
    m_target.SetPrecision(1);
    m_target.SetTextHeight((double)m_target.GetBoundingBox().Height() * 0.9);
    m_target.SetWrap(true);

    m_threshold.SetBoundingBox(grid.GetCell(1, 0));
    m_threshold.SetMin(0.0);
    m_threshold.SetMax(128.0);
    m_threshold.SetStep(0.5);
    m_threshold.SetPrecision(1);
    m_threshold.SetTextHeight((double)m_threshold.GetBoundingBox().Height() * 0.9);
    m_threshold.SetWrap(true);
}
