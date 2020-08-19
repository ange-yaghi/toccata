#ifndef TOCCATA_CORE_DECISION_TREE_H
#define TOCCATA_CORE_DECISION_TREE_H

#include "music_segment.h"
#include "library.h"
#include "full_solver.h"

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace toccata {

    class Bar;

    class DecisionTree {
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

        struct ThreadContext {
            std::thread *Thread;

            FullSolver FullSolver;

            std::mutex Lock;
            std::condition_variable ConditionVariable;

            bool Trigger = false;
            bool Done = false;
            bool Kill = false;

            int LibraryStart = 0;
            int LibraryEnd = 0;

            int DecisionStart = 0;
            int DecisionEnd = 0;
        };

    public:
        DecisionTree();
        ~DecisionTree();

        void SetLibrary(Library *library) { m_library = library; }
        Library *GetLibrary() const { return m_library; }

        void SetInputSegment(const MusicSegment *segment) { m_segment = segment; }
        const MusicSegment *GetInputSegment() const { return m_segment; }

        void SetStartIndex(int index) { m_startIndex = index; }
        int GetStartIndex() const { return m_startIndex; }

        void Initialize(int threadCount);
        void SpawnThreads();
        void KillThreads();
        void Destroy();
        void Process();

    protected:
        void TriggerThreads();

        Decision *AllocateDecision() const { return new Decision; }
        void DestroyDecision(Decision *decision) { delete decision; }

        void WorkerThread(int threadId);
        void SeedMatch(
            int libraryStart, int libraryEnd,
            int threadId);
        void PredictionMatch(
            int decisionIndexStart, int decisionIndexEnd,
            int threadId);

    protected:
        std::vector<Decision *> m_decisions;
        std::queue<Decision *> m_newDecisions;

        ThreadContext *m_threadContexts;

        Library *m_library;
        const MusicSegment *m_segment;
        int m_startIndex;

        int m_threadCount;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_DECISION_TREE_H */
