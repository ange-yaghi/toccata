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

        void SetPosition(const ysVector2 &position) { m_position = position; }
        ysVector2 GetPosition() const { return m_position; }

        void SetSize(const ysVector2 &size) { m_size = size; }
        ysVector2 GetSize() const { return m_size; }

        double GetThreshold() const { return m_threshold.GetCurrentValue(); }
        double GetTarget() const { return m_target.GetCurrentValue(); }

    protected:
        virtual void Render();
        virtual void Update();

    protected:
        ysVector2 m_position;
        ysVector2 m_size;

    protected:
        NumericInput m_target;
        NumericInput m_threshold;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_VELOCITY_PRACTICE_CONTROLS_H */
