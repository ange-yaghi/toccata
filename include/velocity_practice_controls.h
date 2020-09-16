#ifndef TOCCATA_UI_VELOCITY_PRACTICE_CONTROLS_H
#define TOCCATA_UI_VELOCITY_PRACTICE_CONTROLS_H

#include "component.h"

#include "numeric_input.h"

namespace toccata {

    class VelocityPracticeControls : public Component {
    public:
        VelocityPracticeControls();
        ~VelocityPracticeControls();

        virtual void Construct();

        double GetThreshold() const { return m_threshold.GetCurrentValue(); }
        double GetTarget() const { return m_target.GetCurrentValue(); }

    protected:
        virtual void Render();
        virtual void Update();

    protected:
        NumericInput m_target;
        NumericInput m_threshold;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_VELOCITY_PRACTICE_CONTROLS_H */
