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
    if (m_clicked) {
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0x00, 0x00));
    }
    else if (m_mouseHold) {
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0xFF, 0x00));
    }
    else if (m_mouseHover) {
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0x00, 0xFF, 0x00));
    }
    else {
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0x00, 0x00, 0x00));
    }

    const int wx = m_engine->GetScreenWidth();
    const int wy = m_engine->GetScreenHeight();

    m_engine->SetObjectTransform(
        ysMath::TranslationTransform(
            ysMath::LoadVector(m_boundingBox.CenterX() - wx / 2.0, m_boundingBox.CenterY() - wy / 2.0)));
    m_engine->DrawBox(m_boundingBox.Width(), m_boundingBox.Height());

    m_textRenderer->RenderText(
        m_text,
        m_boundingBox.Left() - wx / 2.0,
        m_boundingBox.Top() - wy / 2.0,
        (float)m_textHeight);
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
