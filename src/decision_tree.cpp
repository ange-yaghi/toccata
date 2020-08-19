#include "../include/decision_tree.h"

#include "../include/memory.h"

toccata::DecisionTree::DecisionTree() {
    m_library = nullptr;
    m_segment = nullptr;
    m_startIndex = 0;
    m_threadCount = 0;
}

toccata::DecisionTree::~DecisionTree() {
    /* void */
}

void toccata::DecisionTree::Initialize(int threadCount) {
    m_threadCount = threadCount;

    m_threadContexts = Memory::Allocate<ThreadContext>(m_threadCount);
}

void toccata::DecisionTree::SpawnThreads() {
    for (int i = 0; i < m_threadCount; ++i) {
        m_threadContexts[i].Thread = new std::thread(&DecisionTree::WorkerThread, *this, i);
    }
}

void toccata::DecisionTree::KillThreads() {
    for (int i = 0; i < m_threadCount; ++i) {
        m_threadContexts[i].Kill = true;
    }

    TriggerThreads();

    for (int i = 0; i < m_threadCount; ++i) {
        m_threadContexts[i].Thread->join();
        delete m_threadContexts[i].Thread;

        m_threadContexts[i].Thread = nullptr;
    }
}

void toccata::DecisionTree::Destroy() {
    for (int i = 0; i < m_threadCount; ++i) {
        assert(m_threadContexts[i].Thread == nullptr);
    }

    Memory::Free(m_threadContexts);

    m_threadContexts = nullptr;
}

void toccata::DecisionTree::Process() {
    TriggerThreads();
    
    for (int i = 0; i < m_threadCount; ++i) {
        std::unique_lock<std::mutex> lk(m_threadContexts[i].Lock);
        m_threadContexts[i].ConditionVariable
            .wait(lk, [this, i] { return m_threadContexts[i].Done; });

        m_threadContexts[i].Done = false;
    }
}

void toccata::DecisionTree::TriggerThreads() {
    for (int i = 0; i < m_threadCount; ++i) {
        std::lock_guard<std::mutex> lk(m_threadContexts[i].Lock);
        m_threadContexts[i].Trigger = true;
        m_threadContexts[i].ConditionVariable.notify_one();
    }
}

void toccata::DecisionTree::WorkerThread(int threadId) {
    ThreadContext &context = m_threadContexts[threadId];

    while (!context.Kill) {
        std::unique_lock<std::mutex> lk(context.Lock);
        context.ConditionVariable
            .wait(lk, [this, threadId] { return m_threadContexts[threadId].Trigger; });

        context.Trigger = false;

        SeedMatch(context.LibraryStart, context.LibraryEnd, threadId);
        PredictionMatch(context.DecisionStart, context.DecisionEnd, threadId);

        context.Done = true;

        lk.unlock();
        context.ConditionVariable.notify_one();
    }
}

void toccata::DecisionTree::SeedMatch(
    int libraryStart, int libraryEnd, int threadId) 
{

}

void toccata::DecisionTree::PredictionMatch(
    int decisionIndexStart, int decisionIndexEnd, int threadId) 
{

}
