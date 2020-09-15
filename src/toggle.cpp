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
    m_button.SetSize(m_size);
    m_button.SetPosition(m_position);

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
            color = ysColor::srgbiToLinear(0xFF, 0x88, 0x00);
        }
        else if (m_button.IsHovering()) {
            color = ysColor::srgbiToLinear(0xFF, 0x10, 0x10);
        }
        else {
            color = ysColor::srgbiToLinear(0xFF, 0x00, 0x00);
        }
    }
    else {
        if (m_button.IsHeld()) {
            color = ysColor::srgbiToLinear(0xFF, 0x55, 0x00);
        }
        else if (m_button.IsHovering()) {
            color = ysColor::srgbiToLinear(0xdd, 0xdd, 0xdd);
        }
        else {
            color = ysColor::srgbiToLinear(0x55, 0x55, 0x55);
        }
    }

    const int wx = m_engine->GetScreenWidth();
    const int wy = m_engine->GetScreenHeight();

    m_engine->SetBaseColor(color);

    m_engine->SetObjectTransform(
        ysMath::TranslationTransform(
            ysMath::LoadVector(
                m_position.x + m_size.x / 2 - wx / 2.0,
                m_position.y - m_size.y / 2 - wy / 2.0)));
    m_engine->DrawBox(m_size.x, m_size.y);

    const double margin = m_textHeight * 0.1;

    m_textRenderer->RenderText(
        m_text,
        m_position.x - wx / 2.0 + margin,
        m_position.y - m_size.y / 2 - m_textHeight / 2 - wy / 2.0 + margin,
        (float)m_textHeight);
}
