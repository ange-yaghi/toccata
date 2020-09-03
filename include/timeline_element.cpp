#include "../include/timeline_element.h"

toccata::TimelineElement::TimelineElement() {
    m_timeline = nullptr;
    m_textRenderer = nullptr;
    m_positionY = 0.0;
    m_height = 0.0;
}

toccata::TimelineElement::~TimelineElement() {
    /* void */
}
