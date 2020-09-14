#include "../include/numeric_input.h"

#include <sstream>

toccata::NumericInput::NumericInput() {
    m_step = 0.01;
    m_min = 0.0;
    m_max = 100.0;
    m_currentValue = 0.0;
    m_textHeight = 32.0;
    m_precision = 2;

    m_active = false;
    m_disabled = true;
    m_wrap = true;

    m_position = ysVector2(100, 100);
    m_size = ysVector2(200, 40);
}

toccata::NumericInput::~NumericInput() {
    /* void */
}

void toccata::NumericInput::Initialize(
    dbasic::DeltaEngine *engine, dbasic::TextRenderer *renderer, Settings *settings) 
{
    Component::Initialize(engine, renderer, settings);

    m_button.Initialize(engine, renderer, settings);
}

void toccata::NumericInput::Process() {
    m_button.SetPosition(m_position);
    m_button.SetSize(m_size);
    m_button.Process();

    if (m_button.ProcessClick()) {
        m_active = true;
    }

    if (m_active) {
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

void toccata::NumericInput::Render() {
    const ysVector innerColor = ysColor::srgbiToLinear(0x00, 0x00, 0x00);
    const ysVector enabledOuterColor = ysColor::srgbiToLinear(0xFF, 0xFF, 0x00);
    const ysVector disabledOuterColor = ysColor::srgbiToLinear(0x44, 0x44, 0x44);

    const int wx = m_engine->GetScreenWidth();
    const int wy = m_engine->GetScreenHeight();

    if (m_active) m_engine->SetBaseColor(enabledOuterColor);
    else m_engine->SetBaseColor(disabledOuterColor);

    m_engine->SetObjectTransform(
        ysMath::TranslationTransform(
            ysMath::LoadVector(
                m_position.x + m_size.x / 2 - wx / 2.0, 
                m_position.y - m_size.y / 2 - wy / 2.0)));
    m_engine->DrawBox(m_size.x, m_size.y);

    m_engine->SetBaseColor(innerColor);
    m_engine->SetObjectTransform(
        ysMath::TranslationTransform(
            ysMath::LoadVector(
                m_position.x + m_size.x / 2 - wx / 2.0,
                m_position.y - m_size.y / 2 - wy / 2.0)));
    m_engine->DrawBox(m_size.x - 2.0f, m_size.y - 2.0f);

    std::stringstream ss;
    ss.precision(m_precision);
    ss << m_currentValue;

    m_textRenderer->RenderText(
        ss.str(),
        m_position.x - wx / 2.0,
        m_position.y - m_size.y / 2 - m_textHeight / 2 - wy / 2.0,
        (float)m_textHeight);
}
