#ifndef TOCCATA_CORE_DECISION_THREAD_H
#define TOCCATA_CORE_DECISION_THREAD_H

#include "decision_tree.h"

#include "music_segment.h"

#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

namespace toccata {

    class DecisionThread {
    public:
        DecisionThread();
        ~DecisionThread();

        void Initialize(Library *library, int threadCount, double pulseUnit, double pulseRate);
        void StartThreads();
        void KillThreads();
        void Destroy();

        void DoIteration();
        void RunThread();

        void AddNote(const MusicPoint &point);

        bool IsComplete() { return m_complete; }

        DecisionTree *GetTree() { return &m_tree; }

        void Clear();
        std::vector<DecisionTree::MatchedPiece> GetPieces();

        void RecordIndex(int index);
        int ReadPeakIndex();

        void RecordTargetIndex(int index);
        int ReadPeakTargetIndex();

        void RecordLatency(double latency);
        double ReadPeakLatency();

    protected:
        std::mutex m_bufferLock;

    protected:
        MusicSegment m_inputBuffer;

        int m_currentIndex;
        std::atomic<bool> m_complete;
        std::atomic<bool> m_kill;
        std::thread m_thread;

        DecisionTree m_tree;

    protected:
        // Metrics
        bool m_peakIndexReset;
        int m_peakIndex;
        
        bool m_peakTargetIndexReset;
        int m_peakTargetIndex;

        bool m_peakLatencyReset;
        double m_peakLatency;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_DECISION_THREAD_H */
