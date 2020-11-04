#include "../include/practice_mode_panel.h"

#include "../include/grid.h"

toccata::PracticeModePanel::PracticeModePanel() {
    m_mode = MidiDisplay::PracticeMode::Default;
}

toccata::PracticeModePanel::~PracticeModePanel() {
    /* void */
}

void toccata::PracticeModePanel::Construct() {
    AddChild(&m_timingPracticeControls);
    AddChild(&m_velocityPracticeControls);
    AddChild(&m_timingToggle);
    AddChild(&m_velocityToggle);

    m_practiceModeGroup.AddToggle(&m_timingToggle);
    m_practiceModeGroup.AddToggle(&m_velocityToggle);
}

const toccata::TimingPracticeControls &toccata::PracticeModePanel::GetTimingPracticeControls() const {
    return m_timingPracticeControls;
}

const toccata::VelocityPracticeControls &toccata::PracticeModePanel::GetVelocityPracticeControls() const {
    return m_velocityPracticeControls;
}

void toccata::PracticeModePanel::Render() {
    /* void */
}

void toccata::PracticeModePanel::Update() {
    Grid grid(m_boundingBox, 7, 4, 5.0f);
    Grid buttonGrid(grid.GetFullRange(0, 0, 1, 2), 2, 1, 5.0f, 5.0f);

    m_timingToggle.SetBoundingBox(buttonGrid.GetCell(0, 0));
    m_timingToggle.SetText("T");

    m_velocityToggle.SetBoundingBox(buttonGrid.GetCell(1, 0));
    m_velocityToggle.SetText("V");

    m_timingPracticeControls.SetBoundingBox(grid.GetFullRange(1, 2, 1, 2));
    m_velocityPracticeControls.SetBoundingBox(grid.GetFullRange(3, 4, 1, 2));

    if (m_timingToggle.GetChecked()) {
        m_mode = MidiDisplay::PracticeMode::Timing;
    }
    else if (m_velocityToggle.GetChecked()) {
        m_mode = MidiDisplay::PracticeMode::Velocity;
    }
    else {
        m_mode = MidiDisplay::PracticeMode::Default;
    }

    switch (m_mode) {
    case MidiDisplay::PracticeMode::Timing:
        m_timingPracticeControls.SetEnabled(true);
        m_velocityPracticeControls.SetEnabled(false);
        break;
    case MidiDisplay::PracticeMode::Velocity:
        m_timingPracticeControls.SetEnabled(false);
        m_velocityPracticeControls.SetEnabled(true);
        break;
    default:
        m_timingPracticeControls.SetEnabled(false);
        m_velocityPracticeControls.SetEnabled(false);
        break;
    }

    RenderText("Mode", grid.GetRange(0, 0, 3, 3), 20.0f, 5.0f);
    RenderText("Timing", grid.GetRange(1, 2, 3, 3), 20.0f, 5.0f);
    RenderText("Velocity", grid.GetRange(3, 4, 3, 3), 20.0f, 5.0f);

    RenderText("Min", grid.GetRange(1, 1, 0, 0), 10.0f, 5.0f);
    RenderText("Max", grid.GetRange(2, 2, 0, 0), 10.0f, 5.0f);
    RenderText("Target", grid.GetRange(3, 3, 0, 0), 10.0f, 5.0f);
    RenderText("Range", grid.GetRange(4, 4, 0, 0), 10.0f, 5.0f);
}
