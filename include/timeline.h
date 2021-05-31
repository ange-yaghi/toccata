#ifndef TOCCATA_UI_TIMELINE_H
#define TOCCATA_UI_TIMELINE_H

#include "decision_tree.h"

#include "delta.h"

namespace toccata {

    class Timeline {
    public:
        struct MatchedBar {
            DecisionTree::MatchedBar Bar;
            int Channel;
        };

        struct MatchedPiece {
            Piece *Piece;
            int Channel;

            double InputSpaceStart;
            double InputSpaceEnd;
        };

    public:
        Timeline();
        ~Timeline();

        void SetPositionX(float x) { m_x = x; }
        float GetPositionX() const { return m_x; }

        void SetWidth(float width) { m_width = width; }
        float GetWidth() const { return m_width; }

        void SetInputSegment(MusicSegment *segment) { m_inputSegment = segment; }
        MusicSegment *GetInputSegment() const { return m_inputSegment; }

        void SetUnfinishedSegment(MusicSegment *segment) { m_unfinishedSegment = segment; }
        MusicSegment *GetUnfinishedSegment() const { return m_unfinishedSegment; }

        void SetTimeRange(timestamp range) { m_timeRange = range; }
        timestamp GetTimeRange() const { return m_timeRange; }

        void SetTimeOffset(timestamp offset) { m_timeOffset = offset; }
        timestamp GetTimeOffset() const { return m_timeOffset; }

        void ClearBars() { m_bars.clear(); }
        void AddBar(const DecisionTree::MatchedBar &bar) { m_bars.push_back({ bar }); }

        void ClearPieces() { m_pieces.clear(); }
        void AddPiece(const DecisionTree::MatchedBar &start, const DecisionTree::MatchedBar &end);

        bool InRange(timestamp t) const;
        bool InRange(timestamp start, timestamp end) const;
        void CropToFit(timestamp &start, timestamp &end) const;

        bool InRangeInputSpace(double t) const;
        bool InRangeInputSpace(double start, double end) const;

        double InputSpaceToLocalX(double t) const;
        double InputSpaceToWorldX(double t) const;

        double TimestampToLocalX(timestamp timestamp) const;
        double TimestampToWorldX(timestamp timestamp) const;

        double ReferenceToLocalX(double r, const Transform &T) const;
        double ReferenceToWorldX(double r, const Transform &T) const;

        // Get the timeline start in input space
        double GetBoundaryStart() const;
        double GetBoundaryEnd() const;

        double ReferenceToInputSpace(double r, const Transform &T) const;
        double TimestampToInputSpace(timestamp timestamp) const;

        int GetBarCount() const { return (int)m_bars.size(); }
        MatchedBar &GetBar(int index) { return m_bars[index]; }

        int GetPieceCount() const { return (int)m_pieces.size(); }
        MatchedPiece &GetPiece(int index) { return m_pieces[index]; }

    protected:
        float m_x;
        float m_width;

        timestamp m_timeRange;
        timestamp m_timeOffset;

        MusicSegment *m_inputSegment;
        MusicSegment *m_unfinishedSegment;

        std::vector<MatchedBar> m_bars;
        std::vector<MatchedPiece> m_pieces;

        dbasic::TextRenderer *m_textRenderer;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_TIMELINE_ELEMENT_H */
