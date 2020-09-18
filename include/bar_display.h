#ifndef TOCCATA_UI_BAR_DISPLAY_H
#define TOCCATA_UI_BAR_DISPLAY_H

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

        void AllocateChannels();

        void RenderBarInformation(const Analyzer::BarInformation &info, float x0, float y0, float x1, float y1);

    protected:
        int m_channelCount;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_BAR_DISPLAY_H */
