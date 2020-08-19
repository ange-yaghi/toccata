#ifndef TOCCATA_CORE_DECISION_TREE_H
#define TOCCATA_CORE_DECISION_TREE_H

#include "music_segment.h"
#include "library.h"

#include <vector>
#include <queue>

namespace toccata {

    class Bar;

    class DecisionTree {
    public:
        static constexpr int ThreadCount = 12;

    protected:
        struct Decision {
            int StartIndex;
            int EndIndex;
            Decision *ParentDecision;

            Bar *MatchedBar;
            double s;
            double t;

            double AverageError;
            int MappedNotes;
        };

    public:
        DecisionTree();
        ~DecisionTree();

        void SetLibrary(Library *library) { m_library = library; }
        Library *GetLibrary() const { return m_library; }

        void SetInputSegment(const MusicSegment *segment) { m_segment = segment; }
        const MusicSegment *GetInputSegment() const { return m_segment; }

    protected:
        Decision *AllocateDecision() const { return new Decision; }
        void DestroyDecision(Decision *decision) { delete decision; }

        void WorkerThread(int threadId);
        void SeedMatch(
            const MusicSegment *segment, int startIndex, 
            const Library *library, int libraryStart, int libraryEnd,
            int threadId);
        void PredictionMatch(
            const MusicSegment *segment, int startIndex,
            int decisionIndexStart, int decisionIndexEnd,
            int threadId);

    protected:
        std::vector<Decision *> m_decisions;
        std::queue<Decision *> m_newDecisions;

        Library *m_library;
        const MusicSegment *m_segment;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_DECISION_TREE_H */
