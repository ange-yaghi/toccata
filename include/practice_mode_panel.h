#ifndef TOCCATA_UI_PRACTICE_MODE_PANEL_H
#define TOCCATA_UI_PRACTICE_MODE_PANEL_H

#include "component.h"

#include "timing_practice_controls.h"
#include "velocity_practice_controls.h"
#include "toggle.h"
#include "toggle_group.h"

#include "midi_display.h"

namespace toccata {

    class PracticeModePanel : public Component {
    public:
        PracticeModePanel();
        ~PracticeModePanel();

        virtual void Construct();

        const TimingPracticeControls &GetTimingPracticeControls() const;
        const VelocityPracticeControls &GetVelocityPracticeControls() const;

        MidiDisplay::PracticeMode GetPracticeMode() { return m_mode; }

    protected:
        virtual void Render();
        virtual void Update();

    protected:
        MidiDisplay::PracticeMode m_mode;

        TimingPracticeControls m_timingPracticeControls;
        VelocityPracticeControls m_velocityPracticeControls;

        ToggleGroup m_practiceModeGroup;
        Toggle m_timingToggle;
        Toggle m_velocityToggle;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_PRACTICE_MODE_PANEL_H */
