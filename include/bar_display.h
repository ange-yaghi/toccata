#ifndef TOCCATA_BAR_DISPLAY_H
#define TOCCATA_BAR_DISPLAY_H

#include "decision_tree.h"

#include "delta.h"

namespace toccata {

    class BarDisplay {
    public:
        struct MatchedBar {
            DecisionTree::MatchedBar Bar;
            int Channel;
        };

    public:
        BarDisplay();
        ~BarDisplay();

        void Initialize(dbasic::DeltaEngine *engine);

        void Process();
        void Render();

        void SetPosition(const ysVector &pos) { m_position = pos; }
        ysVector GetPosition() const { return m_position; }

        void SetSize(const ysVector &size) { m_size = size; }
        ysVector GetSize() const { return m_size; }

        void SetChannelCount(int channels) { m_channelCount = channels; }
        int GetChannelCount() const { return m_channelCount; }

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

        void AllocateChannels();

        void DrawBox(float x, float y, float w, float h);
        double GetLocalX(double timestamp) const;

        void SetTextRenderer(dbasic::TextRenderer *renderer) { m_textRenderer = renderer; }
        dbasic::TextRenderer *GetTextRenderer() const { return m_textRenderer; }

        void RenderBarInformation(DecisionTree::MatchedBar *matchedBar, float x0, float y0, float x1, float y1);

    protected:
        ysVector m_position;
        ysVector m_size;

        int m_channelCount;

        double m_timeRange;
        double m_timeOffset;

        dbasic::DeltaEngine *m_engine;

        MusicSegment *m_inputSegment;
        MusicSegment *m_referenceSegment;

        std::vector<MatchedBar> m_bars;

        dbasic::TextRenderer *m_textRenderer;
    };

} /* namespace toccata */

#endif /* TOCCATA_BAR_DISPLAY_H */
