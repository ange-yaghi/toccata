#ifndef TOCCATA_UI_NUMERIC_INPUT_H
#define TOCCATA_UI_NUMERIC_INPUT_H

#include "timeline.h"

#include "component.h"
#include "button.h"

#include "delta.h"

namespace toccata {

    class NumericInput : public Component {
    public:
        NumericInput();
        virtual ~NumericInput();

        virtual void Initialize(dbasic::DeltaEngine *engine, dbasic::TextRenderer *renderer, Settings *settings);

        virtual void Process();
        virtual void Render();

    protected:
        Button m_button;

        ysVector2 m_position;
        ysVector2 m_size;

        double m_textHeight;

        double m_step;
        double m_currentValue;

        double m_min;
        double m_max;

        int m_precision;

        bool m_active;
        bool m_disabled;
        bool m_wrap;
    };

} /* namespace toccata */

#endif /* TOCCATA_NUMERIC_INPUT_H */
