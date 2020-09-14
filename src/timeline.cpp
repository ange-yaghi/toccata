#include "../include/timeline.h"

toccata::Timeline::Timeline() {
    m_x = 0.0;
    m_width = 0.0;

    m_timeRange = 0;
    m_timeOffset = 0;

    m_inputSegment = nullptr;

    m_textRenderer = nullptr;
}

toccata::Timeline::~Timeline() {
    /* void */
}

void toccata::Timeline::AddPiece(const DecisionTree::MatchedBar &start, const DecisionTree::MatchedBar &end) {
    const double startInputSpace = ReferenceToInputSpace(0.0, start.T);
    const double endInputSpace = ReferenceToInputSpace(
        end.MatchedBar->GetSegment()->GetNormalizedLength(), end.T);

    MatchedPiece piece;
    piece.Channel = 0;
    piece.InputSpaceEnd = endInputSpace;
    piece.InputSpaceStart = startInputSpace;
    piece.Piece = start.MatchedBar->GetPiece();

    m_pieces.push_back(piece);
}

bool toccata::Timeline::InRange(timestamp timestamp) const {
    if (timestamp > m_timeOffset + m_timeRange) return false;
    else if (timestamp < m_timeOffset) return false;
    else return true;
}

bool toccata::Timeline::InRange(timestamp start, timestamp end) const {
    if (start > m_timeOffset + m_timeRange) return false;
    else if (end < m_timeOffset) return false;
    else return true;
}

void toccata::Timeline::CropToFit(timestamp &start, timestamp &end) const {
    const timestamp timelineEnd = m_timeOffset + m_timeRange;

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

bool toccata::Timeline::InRangeInputSpace(double t) const {
    if (t < GetBoundaryStart()) return false;
    else if (t > GetBoundaryEnd()) return false;
    else return true;
}

bool toccata::Timeline::InRangeInputSpace(double start, double end) const {
    if (start > GetBoundaryEnd()) return false;
    else if (end < GetBoundaryStart()) return false;
    else return true;
}

double toccata::Timeline::InputSpaceToLocalX(double t) const {
    const double t_space = t * m_inputSegment->PulseUnit;
    const double local = (t_space / m_timeRange) * m_width;

    if (local > m_width) return m_width;
    else if (local < 0.0) return 0.0;
    else return local;
}

double toccata::Timeline::InputSpaceToWorldX(double t) const {
    return InputSpaceToLocalX(t) + m_x;
}

double toccata::Timeline::TimestampToInputSpace(timestamp t) const {
    const timestamp adjustedTimestamp = t - m_timeOffset;
    const double inputSpace = m_inputSegment->Normalize(adjustedTimestamp);

    return inputSpace;
}

double toccata::Timeline::TimestampToLocalX(timestamp t) const {
    const double width = m_width;
    const double local = ((t - m_timeOffset) / (float)m_timeRange) * width;

    if (local > width) return width;
    else if (local < 0.0) return 0.0;
    else return local;
}

double toccata::Timeline::TimestampToWorldX(timestamp timestamp) const {
    return TimestampToLocalX(timestamp) + m_x;
}

double toccata::Timeline::ReferenceToLocalX(double r, const Transform &T) const {
    const double inputSpace = ReferenceToInputSpace(r, T);
    const double t_space = inputSpace * m_inputSegment->PulseUnit;
    const double local = (t_space / m_timeRange) * m_width;

    if (local > m_width) return m_width;
    else if (local < 0.0) return 0.0;
    else return local;
}

double toccata::Timeline::ReferenceToWorldX(double r, const Transform &T) const {
    return m_x + ReferenceToLocalX(r, T);
}

double toccata::Timeline::GetBoundaryStart() const {
    return TimestampToInputSpace(m_timeOffset);
}

double toccata::Timeline::GetBoundaryEnd() const {
    return TimestampToInputSpace(m_timeOffset + m_timeRange);
}

double toccata::Timeline::ReferenceToInputSpace(double r, const Transform &T) const {
    const timestamp dt = T.t_coarse - m_timeOffset;
    const double offset = m_inputSegment->Normalize(dt);
    const double inputSpace = T.inv_f(r);

    return inputSpace + offset;
}
