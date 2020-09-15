#include "../include/practice_mode_panel.h"

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
    m_timingToggle.SetPosition({ 10, 40 });
    m_timingToggle.SetSize({ 30, 30 });
    m_timingToggle.SetText("T");

    m_velocityToggle.SetPosition({ 50, 40 });
    m_velocityToggle.SetSize({ 30, 30 });
    m_velocityToggle.SetText("V");

    m_timingPracticeControls.SetSize({ 200, 40 });
    m_timingPracticeControls.SetPosition({ 100, 50 });

    m_velocityPracticeControls.SetSize({ 200, 40 });
    m_velocityPracticeControls.SetPosition({ 100, 50 });

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
        m_timingPracticeControls.SetVisible(true);
        m_timingPracticeControls.SetEnabled(true);
        m_velocityPracticeControls.SetVisible(false);
        m_velocityPracticeControls.SetEnabled(false);
        break;
    case MidiDisplay::PracticeMode::Velocity:
        m_timingPracticeControls.SetVisible(false);
        m_timingPracticeControls.SetEnabled(false);
        m_velocityPracticeControls.SetVisible(true);
        m_velocityPracticeControls.SetEnabled(true);
        break;
    default:
        m_timingPracticeControls.SetVisible(false);
        m_timingPracticeControls.SetEnabled(false);
        m_velocityPracticeControls.SetVisible(false);
        m_velocityPracticeControls.SetEnabled(false);
        break;
    }
}
