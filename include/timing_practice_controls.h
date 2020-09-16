#ifndef TOCCATA_UI_TIMING_PRACTICE_CONTROLS_H
#define TOCCATA_UI_TIMING_PRACTICE_CONTROLS_H

#include "component.h"

#include "numeric_input.h"

namespace toccata {

    class TimingPracticeControls : public Component {
    public:
        TimingPracticeControls();
        ~TimingPracticeControls();

        virtual void Construct();

        double GetThreshold() const { return m_threshold.GetCurrentValue(); }

    protected:
        virtual void Render();
        virtual void Update();

    protected:
        NumericInput m_threshold;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_TIMING_PRACTICE_CONTROLS_H */
