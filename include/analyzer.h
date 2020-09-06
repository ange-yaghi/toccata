#ifndef TOCCATA_UI_ANALYZER_H
#define TOCCATA_UI_ANALYZER_H

#include "timeline.h"

#include <vector>

namespace toccata {

    class Analyzer {
    public:
        struct NoteInformation {
            int ReferenceNote;
            int InputNote;
            double Error;
        };

        struct BarInformation {
            int Bar;
            std::vector<NoteInformation> NoteInformation;
            double AverageError;
            double Tempo;
        };

    public:
        Analyzer();
        ~Analyzer();

        void SetTimeline(Timeline *timeline) { m_timeline = timeline; }
        Timeline *GetTimeline() const { return m_timeline; }

        int GetBarCount() const { return m_barCount; }
        BarInformation &GetBar(int index) { return m_bars[index]; }

        void Analyze();

    protected:
        bool BarInRange(const Timeline::MatchedBar &bar) const;
        void ProcessBar(const Timeline::MatchedBar &bar, int index, int masterIndex);

        double CalculateTempo(const Timeline::MatchedBar &bar) const;

    protected:
        Timeline *m_timeline;
        BarInformation *m_bars;
        int m_barCount;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_ANALYZER_H */
