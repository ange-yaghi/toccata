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

        void SetPosition(const ysVector2 &position) { m_position = position; }
        ysVector2 GetPosition() const { return m_position; }

        void SetSize(const ysVector2 &size) { m_size = size; }
        ysVector2 GetSize() const { return m_size; }

        const TimingPracticeControls &GetTimingPracticeControls() const;
        const VelocityPracticeControls &GetVelocityPracticeControls() const;

        MidiDisplay::PracticeMode GetPracticeMode() { return m_mode; }

    protected:
        virtual void Render();
        virtual void Update();

        ysVector2 m_position;
        ysVector2 m_size;

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
