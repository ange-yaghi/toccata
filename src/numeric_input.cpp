#include "../include/numeric_input.h"

#include <sstream>

toccata::NumericInput::NumericInput() {
    m_step = 0.01;
    m_min = 0.0;
    m_max = 100.0;
    m_currentValue = 0.0;
    m_textHeight = 32.0;
    m_precision = 2;

    m_disabled = true;
    m_wrap = true;

    m_position = ysVector2(100, 100);
    m_size = ysVector2(200, 40);
}

toccata::NumericInput::~NumericInput() {
    /* void */
}

void toccata::NumericInput::Construct() {
    AddChild(&m_button);

    m_button.SetVisible(false);
}

void toccata::NumericInput::ProcessInput() {
    if (m_button.IsHeld()) {
        RequestControl();
    }

    if (HasControl()) {
        if (m_engine->ProcessKeyDown(ysKeyboard::KEY_UP)) {
            m_currentValue += m_step;
        }
        else if (m_engine->ProcessKeyDown(ysKeyboard::KEY_DOWN)) {
            m_currentValue -= m_step;
        }
    }

    if (m_currentValue > m_max) {
        if (m_wrap) m_currentValue = (m_currentValue - m_max) + m_min;
        else m_currentValue = m_max;
    }
    else if (m_currentValue < m_min) {
        if (m_wrap) m_currentValue = m_max - (m_min - m_currentValue);
        else m_currentValue = m_min;
    }
}

void toccata::NumericInput::Update() {
    m_button.SetPosition(m_position);
    m_button.SetSize(m_size);
}

void toccata::NumericInput::Render() {
    const ysVector innerColor = ysColor::srgbiToLinear(0x00, 0x00, 0x00);
    const ysVector enabledOuterColor = ysColor::srgbiToLinear(0xFF, 0xFF, 0x00);
    const ysVector disabledOuterColor = ysColor::srgbiToLinear(0x44, 0x44, 0x44);

    const ysVector outerColor = HasControl()
        ? enabledOuterColor
        : disabledOuterColor;

    DrawBox(m_position, m_size, outerColor);
    DrawBox({ m_position.x + 1, m_position.y - 1 }, { m_size.x - 2.0f, m_size.y - 2.0f }, innerColor);

    std::stringstream ss;
    ss.precision(m_precision);
    ss << m_currentValue;

    RenderText(
        ss.str(),
        { m_position.x, m_position.y - m_size.y / 2 - (float)m_textHeight / 2 }, 
        (float)m_textHeight);
}
