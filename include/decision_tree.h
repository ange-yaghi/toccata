#ifndef TOCCATA_CORE_DECISION_TREE_H
#define TOCCATA_CORE_DECISION_TREE_H

#include "music_segment.h"
#include "library.h"
#include "full_solver.h"
#include "bar.h"

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace toccata {

    class DecisionTree {
    protected:
        static constexpr double DefaultMargin = 0.25;

    protected:
        struct Decision {
            int MappingStart;
            int MappingEnd;;
            Decision *ParentDecision;

            const Bar *MatchedBar;
            double s;
            double t;

            double AverageError;
            int MappedNotes;

            bool Flagged = false;

            bool IsSameAs(const Decision *decision) const;
            bool IsBetterFitThan(const Decision *decision) const;

            int GetDepth() const;
        };

        struct ThreadContext {
            std::thread *Thread;

            FullSolver Solver;

            std::mutex Lock;
            std::condition_variable ConditionVariable;

            bool Trigger = false;
            bool Done = false;
            bool Kill = false;

            int LibraryStart = 0;
            int LibraryEnd = 0;

            int DecisionStart = 0;
            int DecisionEnd = 0;

            int StartIndex = 0;

            std::queue<Decision *> NewDecisions;
        };

    public:
        DecisionTree();
        ~DecisionTree();

        void SetMargin(double margin) { m_margin = margin; }
        double GetMargin() const { return m_margin; }

        void SetLibrary(Library *library) { m_library = library; }
        Library *GetLibrary() const { return m_library; }

        void SetInputSegment(const MusicSegment *segment) { m_segment = segment; }
        const MusicSegment *GetInputSegment() const { return m_segment; }

        int GetDecisionCount() const { return (int)m_decisions.size(); }

        void Initialize(int threadCount);
        void SpawnThreads();
        void KillThreads();
        void Destroy();
        void Process(int startIndex);

    protected:
        void DistributeWork();
        void TriggerThreads();
        void WaitForThreads();
        void Integrate();
        void Prune();

        bool IntegrateDecision(Decision *decision);

        Decision *AllocateDecision() const { return new Decision; }
        void DestroyDecision(Decision *decision) { delete decision; }

        void WorkerThread(int threadId);
        void Work(int threadId, ThreadContext &context);
        void SeedMatch(
            int libraryStart, int libraryEnd,
            int threadId);
        void PredictionMatch(
            int decisionIndexStart, int decisionIndexEnd,
            int threadId);
        Decision *Match(const Bar *bar, ThreadContext &context);

    protected:
        std::vector<Decision *> m_decisions;

        ThreadContext *m_threadContexts;

        Library *m_library;
        const MusicSegment *m_segment;

        int m_threadCount;

        double m_margin = DefaultMargin;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_DECISION_TREE_H */
