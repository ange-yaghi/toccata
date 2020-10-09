#include "../include/piece_display.h"

#include "../include/transform.h"

#include <sstream>

toccata::PieceDisplay::PieceDisplay() {
    m_channelCount = 3;
}

toccata::PieceDisplay::~PieceDisplay() {
    /* void */
}

void toccata::PieceDisplay::Initialize(dbasic::DeltaEngine *engine) {
    m_engine = engine;
}

void toccata::PieceDisplay::Process() {
    /* void */
}

void toccata::PieceDisplay::Render() {
    AllocateChannels();

    if (m_channelCount == 0) return;

    const float totalHeight = m_height;
    const float totalWidth = m_timeline->GetWidth();

    const float channelHeight = totalHeight / m_channelCount;

    const float cornerX = m_timeline->GetPositionX();
    const float cornerY = m_positionY;

    const int n = m_timeline->GetPieceCount();

    for (int i = 0; i < n; ++i) {
        const Timeline::MatchedPiece &piece = m_timeline->GetPiece(i);

        const double s = piece.InputSpaceStart;
        const double e = piece.InputSpaceEnd;

        if (!m_timeline->InRangeInputSpace(s, e)) continue;

        const double xs = m_timeline->InputSpaceToWorldX(s);
        const double xe = m_timeline->InputSpaceToWorldX(e);

        const float y = cornerY - (m_channelCount - piece.Channel - 1) * channelHeight;
        const float x = (float)xs;
        const float width = (float)(xe - xs);

        DrawBox(BoundingBox(width, channelHeight)
            .AlignLeft(x)
            .AlignTop(y), m_settings->PieceDisplay_BackgroundColor);

        RenderPieceInformation(piece.Piece->GetName(), x, y - channelHeight, x + channelHeight, y);
    }
}

void toccata::PieceDisplay::AllocateChannels() {
    m_channelCount = m_settings->PieceDisplay_MinimumChannelCount;

    const int pieceCount = m_timeline->GetPieceCount();
    for (int i = 0; i < pieceCount; ++i) {
        Timeline::MatchedPiece &b0 = m_timeline->GetPiece(i);

        const double s0 = b0.InputSpaceStart;
        const double e0 = b0.InputSpaceEnd;

        if (!m_timeline->InRangeInputSpace(s0, e0)) continue;

        std::set<int> conflicts;
        for (int j = 0; j < i; ++j) {
            const Timeline::MatchedPiece &b1 = m_timeline->GetPiece(j);

            const double s1 = b1.InputSpaceStart;
            const double e1 = b1.InputSpaceEnd;

            if (!m_timeline->InRangeInputSpace(s1, e1)) continue;

            if (s0 > e1 || e0 < s1) continue;
            if (s1 > e0 || e1 < s0) continue;

            const int channel = b1.Channel;
            conflicts.insert(channel);
        }

        for (int c = 0; c <= m_channelCount; ++c) {
            if (c == m_channelCount) ++m_channelCount;

            if (conflicts.count(c) == 0) {
                b0.Channel = c;
                break;
            }
        }
    }
}

void toccata::PieceDisplay::RenderPieceInformation(
    const std::string &name, float x0, float y0, float x1, float y1) 
{
    RenderText(name, { x0, y0 }, 20.0f);
}
