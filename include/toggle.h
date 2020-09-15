#ifndef TOCCATA_UI_TOGGLE_H
#define TOCCATA_UI_TOGGLE_H

#include "timeline.h"

#include "component.h"
#include "button.h"
#include "toggle_group.h"

#include "delta.h"

namespace toccata {

    class Toggle : public Component {
    public:
        Toggle();
        virtual ~Toggle();

        virtual void Construct();

        void SetPosition(const ysVector2 position) { m_position = position; }
        ysVector2 GetPosition() const { return m_position; }

        void SetSize(const ysVector2 size) { m_size = size; }
        ysVector2 GetSize() const { return m_size; }

        void SetTextHeight(double textHeight) { m_textHeight = textHeight; }
        double GetTextHeight() const { return m_textHeight; }

        void SetText(const std::string &text) { m_text = text; }
        std::string GetText() const { return m_text; }

        void SetToggleGroup(ToggleGroup *group) { m_group = group; }
        ToggleGroup *GetToggleGroup() const { return m_group; }

        void SetChecked(bool checked) { m_checked = checked; }
        bool GetChecked() const { return m_checked; }

    protected:
        virtual void Update();
        virtual void ProcessInput();
        virtual void Render();

    protected:
        ToggleGroup *m_group;
        Button m_button;

        ysVector2 m_position;
        ysVector2 m_size;

        std::string m_text;

        double m_textHeight;

        bool m_checked;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_TOGGLE_H */
