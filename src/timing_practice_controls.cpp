#include "../include/timing_practice_controls.h"
#include "../include/grid.h"

toccata::TimingPracticeControls::TimingPracticeControls() {
    /* void */
}

toccata::TimingPracticeControls::~TimingPracticeControls() {
    /* void */
}

void toccata::TimingPracticeControls::Construct() {
    AddChild(&m_minError);
    AddChild(&m_maxError);
}

void toccata::TimingPracticeControls::Render() {
    Component::Render();
}

void toccata::TimingPracticeControls::Update() {
    Grid grid;
    grid.SetBoundingBox(m_boundingBox);
    grid.SetVerticalDivisions(1);
    grid.SetHorizontalDivisions(2);
    grid.SetVerticalMargin(5.0f);
    grid.SetHorizontalMargin(5.0f);

    m_minError.SetBoundingBox(grid.GetCell(0, 0));
    m_minError.SetMin(0.0);
    m_minError.SetMax(1000.0);
    m_minError.SetStep(0.5);
    m_minError.SetPrecision(1);
    m_minError.SetTextHeight((double)m_minError.GetBoundingBox().Height() * 0.9);
    m_minError.SetWrap(true);

    m_maxError.SetBoundingBox(grid.GetCell(1, 0));
    m_maxError.SetMin(0.0);
    m_maxError.SetMax(1000.0);
    m_maxError.SetStep(0.5);
    m_maxError.SetPrecision(1);
    m_maxError.SetTextHeight((double)m_maxError.GetBoundingBox().Height() * 0.9);
    m_maxError.SetWrap(true);

    if (m_minError.GetCurrentValue() > m_maxError.GetCurrentValue()) {
        m_minError.SetCurrentValue(m_maxError.GetCurrentValue());
    }

    if (m_maxError.GetCurrentValue() < m_minError.GetCurrentValue()) {
        m_maxError.SetCurrentValue(m_minError.GetCurrentValue());
    }
}
