#ifndef TOCCATA_BAR_DISPLAY_H
#define TOCCATA_BAR_DISPLAY_H

#include "timeline.h"

#include "decision_tree.h"
#include "timeline_element.h"

#include "delta.h"

namespace toccata {

    class BarDisplay : public TimelineElement {
    public:
        BarDisplay();
        ~BarDisplay();

        void Initialize(dbasic::DeltaEngine *engine);

        void Process();
        void Render();

        void SetMinimumChannelCount(int channels) { m_minimumChannelCount = channels; }
        int GetMinimumChannelCount() const { return m_minimumChannelCount; }

        void AllocateChannels();

        void DrawBox(float x, float y, float w, float h);

        void RenderBarInformation(const DecisionTree::MatchedBar *matchedBar, float x0, float y0, float x1, float y1);

    protected:
        int m_minimumChannelCount;
        int m_channelCount;
    };

} /* namespace toccata */

#endif /* TOCCATA_BAR_DISPLAY_H */
