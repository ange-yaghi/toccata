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

    public:
        Timeline();
        ~Timeline();

        void SetPositionX(float x) { m_x = x; }
        float GetPositionX() const { return m_x; }

        void SetWidth(float width) { m_width = width; }
        float GetWidth() const { return m_width; }

        void SetInputSegment(MusicSegment *segment) { m_inputSegment = segment; }
        MusicSegment *GetInputSegment() const { return m_inputSegment; }

        void SetReferenceSegment(MusicSegment *segment) { m_referenceSegment = segment; }
        MusicSegment *GetReferenceSegment() const { return m_referenceSegment; }

        void SetTimeRange(double range) { m_timeRange = range; }
        double GetTimeRange() const { return m_timeRange; }

        void SetTimeOffset(double offset) { m_timeOffset = offset; }
        double GetTimeOffset() const { return m_timeOffset; }

        void ClearBars() { m_bars.clear(); }
        void AddBar(const DecisionTree::MatchedBar &bar) { m_bars.push_back({ bar }); }

        bool InRange(double timestamp) const;
        bool InRange(double start, double end) const;
        void CropToFit(double &start, double &end) const;

        double GetLocalX(double timestamp) const;
        double GetWorldX(double timestamp) const;

        int GetBarCount() const { return (int)m_bars.size(); }
        MatchedBar &GetBar(int index) { return m_bars[index]; }

    protected:
        float m_x;
        float m_width;

        double m_timeRange;
        double m_timeOffset;

        MusicSegment *m_inputSegment;
        MusicSegment *m_referenceSegment;

        std::vector<MatchedBar> m_bars;

        dbasic::TextRenderer *m_textRenderer;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_TIMELINE_ELEMENT_H */
