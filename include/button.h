#ifndef TOCCATA_BUTTON_H
#define TOCCATA_BUTTON_H

#include "timeline.h"

#include "decision_tree.h"
#include "timeline_element.h"
#include "component.h"

#include "delta.h"

namespace toccata {

    class Button : public Component {
    public:
        Button();
        virtual ~Button();

        bool IsHovering() const { return m_mouseHover; }
        bool IsHeld() const { return m_mouseHold; }
        bool ProcessClick();

        void SetTextHeight(double textHeight) { m_textHeight = textHeight; }
        double GetTextHeight() const { return m_textHeight; }

        void SetText(const std::string &text) { m_text = text; }
        std::string GetText() const { return m_text; }

    protected:
        virtual void Update();
        virtual void ProcessInput();
        virtual void Render();

        std::string m_text;

        double m_textHeight;

        bool m_mouseHover;
        bool m_mouseHold;
        bool m_clicked;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_BUTTON_H */
