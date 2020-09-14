#include "../include/component.h"

toccata::Component::Component() {
    m_engine = nullptr;
    m_textRenderer = nullptr;

    m_settings = nullptr;
}

toccata::Component::~Component() {
    /* void */
}

void toccata::Component::Initialize(dbasic::DeltaEngine *engine, dbasic::TextRenderer *renderer, Settings *settings) {
    m_engine = engine;
    m_textRenderer = renderer;
    m_settings = settings;
}
