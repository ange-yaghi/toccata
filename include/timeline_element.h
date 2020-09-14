#ifndef TOCCATA_UI_TIMELINE_ELEMENT_H
#define TOCCATA_UI_TIMELINE_ELEMENT_H

#include "timeline.h"
#include "analyzer.h"
#include "settings.h"
#include "component.h"

namespace toccata {

    class TimelineElement : public Component {
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

    protected:
        Timeline *m_timeline;
        Analyzer *m_analyzer;

        float m_positionY;
        float m_height;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_TIMELINE_ELEMENT_H */
