#include "../include/toggle.h"

toccata::Toggle::Toggle() {
    m_textHeight = 32.0;
    m_checked = false;
    m_text = "";
    m_group = nullptr;
}

toccata::Toggle::~Toggle() {
    /* void */
}

void toccata::Toggle::Construct() {
    AddChild(&m_button);

    m_button.SetVisible(false);
}

void toccata::Toggle::Update() {
    /* void */
}

void toccata::Toggle::ProcessInput() {
    m_button.SetBoundingBox(m_boundingBox);

    if (m_button.ProcessClick()) {
        m_checked = !m_checked;

        if (m_checked) {
            m_group->SetOn(this);
        }
    }
}

void toccata::Toggle::Render() {
    ysVector color;
    if (m_checked) {
        if (m_button.IsHeld()) {
            color = m_settings->Toggle_CheckedHoldColor;
        }
        else if (m_button.IsHovering()) {
            color = m_settings->Toggle_CheckedHoverColor;
        }
        else {
            color = m_settings->Toggle_CheckedColor;
        }
    }
    else {
        if (m_button.IsHeld()) {
            color = m_settings->Toggle_UncheckedHoldColor;
        }
        else if (m_button.IsHovering()) {
            color = m_settings->Toggle_UncheckedHoverColor;
        }
        else {
            color = m_settings->Toggle_UncheckedColor;
        }
    }

    const double margin = m_textHeight * 0.1;

    DrawBox(m_boundingBox, color);
    RenderText(
        m_text,
        { m_boundingBox.Left() + (float)margin, m_boundingBox.CenterY() - (float)m_textHeight / 2 + (float)margin },
        m_textHeight);
}
