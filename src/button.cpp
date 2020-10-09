#include "../include/button.h"

toccata::Button::Button() {
    m_mouseHover = false;
    m_mouseHold = false;
    m_clicked = false;

    m_text = "";
    m_textHeight = 32.0;
}

toccata::Button::~Button() {
    /* void */
}

void toccata::Button::Update() {
    /* void */
}

void toccata::Button::ProcessInput() {
    int mx, my;
    m_engine->GetOsMousePos(&mx, &my);

    m_mouseHover = false;

    if (mx > m_boundingBox.Left() && mx < m_boundingBox.Right()) {
        if (my < m_boundingBox.Top() && my > m_boundingBox.Bottom()) {
            m_mouseHover = true;
        }
    }

    if (m_mouseHover && m_engine->ProcessMouseKeyDown(ysMouse::Button::Left)) {
        m_mouseHold = true;
    }

    if (m_mouseHold && !m_engine->IsMouseKeyDown(ysMouse::Button::Left)) {
        m_mouseHold = false;
        m_clicked = m_mouseHover;
    }
}

void toccata::Button::Render() {
    ysVector color;
    if (m_clicked) {
        color = m_settings->Button_ClickedColor;
    }
    else if (m_mouseHold) {
        color = m_settings->Button_HoldColor;
    }
    else if (m_mouseHover) {
        color = m_settings->Button_HoverColor;
    }
    else {
        color = m_settings->Button_DefaultColor;
    }

    DrawBox(m_boundingBox, color);
    RenderText(m_text, { m_boundingBox.Left(), m_boundingBox.Top() }, (float)m_textHeight);
}

bool toccata::Button::ProcessClick() {
    if (m_clicked) {
        m_clicked = false;
        return true;
    }
    else {
        return false;
    }
}
