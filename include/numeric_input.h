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

        virtual void Construct();

        void SetPosition(const ysVector2 position) { m_position = position; }
        ysVector2 GetPosition() const { return m_position; }

        void SetSize(const ysVector2 size) { m_size = size; }
        ysVector2 GetSize() const { return m_size; }

        void SetTextHeight(double textHeight) { m_textHeight = textHeight; }
        double GetTextHeight() const { return m_textHeight; }

        void SetStep(double step) { m_step = step; }
        double GetStep() const { return m_step; }

        void SetCurrentValue(double currentValue) { m_currentValue = currentValue; }
        double GetCurrentValue() const { return m_currentValue; }

        void SetMin(double min) { m_min = min; }
        double GetMin() const { return m_min; }

        void SetMax(double max) { m_max = max; }
        double GetMax() const { return m_max; }

        void SetPrecision(int precision) { m_precision = precision; }
        int GetPrecision() const { return m_precision; }

        void SetDisabled(bool disabled) { m_disabled = disabled; }
        bool GetDisabled() const { return m_disabled; }

        void SetWrap(bool wrap) { m_wrap = wrap; }
        bool GetWrap() const { return m_wrap; }

    protected:
        virtual void Update();
        virtual void ProcessInput();
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

        bool m_disabled;
        bool m_wrap;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_NUMERIC_INPUT_H */
