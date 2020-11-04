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

        double GetMinError() const { return m_minError.GetCurrentValue() / 1000.0; }
        double GetMaxError() const { return m_maxError.GetCurrentValue() / 1000.0; }

    protected:
        virtual void Render();
        virtual void Update();

    protected:
        NumericInput m_minError;
        NumericInput m_maxError;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_TIMING_PRACTICE_CONTROLS_H */
