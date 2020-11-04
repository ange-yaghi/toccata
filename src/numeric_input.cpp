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
    m_button.SetBoundingBox(m_boundingBox);
}

void toccata::NumericInput::Render() {
    const ysVector innerColor = m_settings->NumericInput_InnerColor;
    const ysVector enabledOuterColor = m_settings->NumericInput_EnabledOuterColor;
    const ysVector disabledOuterColor = m_settings->NumericInput_DisabledOuterColor;

    const ysVector outerColor = HasControl()
        ? enabledOuterColor
        : disabledOuterColor;

    DrawBox(m_boundingBox, outerColor);
    DrawBox(m_boundingBox.MarginOffset(-1.0f, -1.0f), innerColor);

    std::stringstream ss;
    ss.precision(m_precision);
    ss << std::fixed << m_currentValue;

    RenderText(
        ss.str(),
        { m_boundingBox.Left(), m_boundingBox.CenterY() - (float)m_textHeight / 2 }, 
        (float)m_textHeight);
}
