#include "../include/timeline.h"

toccata::Timeline::Timeline() {
    m_x = 0.0;
    m_width = 0.0;

    m_timeRange = 0.0;
    m_timeOffset = 0.0;

    m_inputSegment = nullptr;
    m_referenceSegment = nullptr;

    m_textRenderer = nullptr;
}

toccata::Timeline::~Timeline() {
    /* void */
}

bool toccata::Timeline::InRange(double timestamp) const {
    if (timestamp > m_timeOffset + m_timeRange) return false;
    else if (timestamp < m_timeOffset) return false;
    else return true;
}

bool toccata::Timeline::InRange(double start, double end) const {
    if (start > m_timeOffset + m_timeRange) return false;
    else if (end < m_timeOffset) return false;
    else return true;
}

void toccata::Timeline::CropToFit(double &start, double &end) const {
    const double timelineEnd = m_timeOffset + m_timeRange;

    if (start > timelineEnd) {
        start = timelineEnd;
        end = timelineEnd;
    }
    else if (end > m_timeOffset) {
        start = m_timeOffset;
        end = m_timeOffset;
    }
    else if (start < m_timeOffset) start = m_timeOffset;
    else if (end > m_timeOffset + m_timeRange) end = m_timeOffset + m_timeRange;
}

double toccata::Timeline::GetLocalX(double timestamp) const {
    const float width = m_width;
    const float local = ((timestamp - m_timeOffset) / m_timeRange) * width;

    if (local > width) return width;
    else if (local < 0.0) return 0.0;
    else return local;
}

double toccata::Timeline::GetWorldX(double timestamp) const {
    return GetLocalX(timestamp) + m_x;
}
