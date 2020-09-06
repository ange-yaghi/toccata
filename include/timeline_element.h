#ifndef TOCCATA_UI_TIMELINE_ELEMENT_H
#define TOCCATA_UI_TIMELINE_ELEMENT_H

#include "timeline.h"
#include "analyzer.h"

namespace toccata {

    class TimelineElement {
    public:
        TimelineElement();
        ~TimelineElement();

        void SetTimeline(Timeline *timeline) { m_timeline = timeline; }
        Timeline *GetTimeline() const { return m_timeline; }

        void SetAnalyzer(Analyzer *analyzer) { m_analyzer = analyzer; }
        Analyzer *GetAnalyzer() const { return m_analyzer; }

        float GetPositionY() const { return m_positionY; }
        void SetPositionY(float y) { m_positionY = y; }

        float GetHeight() const { return m_height; }
        void SetHeight(float height) { m_height = height; }

        void SetTextRenderer(dbasic::TextRenderer *renderer) { m_textRenderer = renderer; }
        dbasic::TextRenderer *GetTextRenderer() const { return m_textRenderer; }

        void SetEngine(dbasic::DeltaEngine *engine) { m_engine = engine; }
        dbasic::DeltaEngine *GetEngine() const { return m_engine; }

    protected:
        Timeline *m_timeline;
        Analyzer *m_analyzer;

        float m_positionY;
        float m_height;

        dbasic::DeltaEngine *m_engine;
        dbasic::TextRenderer *m_textRenderer;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_ELEMENT_H */
