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
#include <set>

namespace toccata {

    class DecisionTree {
    protected:
        static constexpr double DefaultMargin = 0.25;
        static constexpr bool ForceMultithreaded = false;

    public:
        struct MatchedBar {
            const Bar *MatchedBar;

            int Start;
            int End;

            double s;
            double t;
        };

        struct MatchedPiece {
            int Start;
            int End;

            std::vector<MatchedBar> Bars;
        };

        struct Decision {
            const Bar *MatchedBar;

            int Index;

            std::set<int> Notes;
            int MappedNotes;

            bool Cached = false;
            int Depth;
            Decision *ParentDecision;

            std::vector<Decision *> OverlappingDecisions;

            double s;
            double t;

            double AverageError;

            bool IsSameAs(const Decision *decision) const;
            bool IsBetterFitThan(const Decision *decision) const;

            bool IsCached() const;
            void InvalidateCache();

            int GetFootprint() const;
            int GetEnd() const;
            int GetStart() const;

            bool Overlapping(const Decision *decision, int overlap) const;
        };

    protected:
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

        Decision *GetDecision(int index) { return m_decisions[index]; }
        int GetDecisionCount() const { return (int)m_decisions.size(); }

        void InvalidateAfter(int index);
        void OnNoteChange(int changedNote);

        void Initialize(int threadCount);
        void SpawnThreads();
        void KillThreads();
        void Destroy();
        void Process(int startIndex);

        int GetDepth(Decision *decision) const;
        int GetBranchNoteCount(Decision *decision) const;
        double GetBranchAverageError(Decision *decision) const;
        int GetBranchStart(Decision *decision) const;
        int GetBranchEnd(Decision *decision);

        void Clear();
        std::vector<MatchedPiece> GetPieces();

        void Prune();

    protected:
        void DistributeWork();
        void TriggerThreads();
        void WaitForThreads();
        void Integrate();

        void UpdateDecision(Decision *target, Decision *source);
        void DeleteDecision(Decision *decision);
        void UpdateOverlapMatrix(Decision *decision);
        void CleanOverlapMatrix(Decision *decision);

        Decision *FindBestParent(const Decision *decision) const;

        bool IntegrateDecision(Decision *decision);

        Decision *AllocateDecision() const { return new Decision; }
        void DestroyDecision(Decision *decision) { delete decision; }

        void WorkerThread(int threadId);
        void Work(int threadId, ThreadContext &context);
        void SeedMatch(
            int libraryStart, int libraryEnd,
            int threadId);
        Decision *Match(const Bar *bar, int startIndex, ThreadContext &context);

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
