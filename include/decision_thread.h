#ifndef TOCCATA_CORE_DECISION_THREAD_H
#define TOCCATA_CORE_DECISION_THREAD_H

#include "decision_tree.h"

#include "music_segment.h"

#include <mutex>
#include <atomic>
#include <thread>

namespace toccata {

    class DecisionThread {
    public:
        DecisionThread();
        ~DecisionThread();

        void Initialize(Library *library, int threadCount);
        void StartThreads();
        void KillThreads();
        void Destroy();

        void DoIteration();
        void RunThread();

        void AddNote(const MusicPoint &point);

        bool IsComplete() { return m_complete; }

        DecisionTree *GetTree() { return &m_tree; }

    protected:
        std::mutex m_bufferLock;

    protected:
        MusicSegment m_inputBuffer;

        int m_currentIndex;
        std::atomic<bool> m_complete;
        std::atomic<bool> m_kill;
        std::thread m_thread;

        DecisionTree m_tree;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_DECISION_THREAD_H */
