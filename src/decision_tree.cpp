#include "../include/decision_tree.h"

#include "../include/memory.h"

toccata::DecisionTree::DecisionTree() {
    m_library = nullptr;
    m_segment = nullptr;
    m_threadCount = 0;
}

toccata::DecisionTree::~DecisionTree() {
    /* void */
}

void toccata::DecisionTree::Initialize(int threadCount) {
    m_threadCount = threadCount;
    m_threadContexts = Memory::Allocate<ThreadContext>(m_threadCount);

    for (int i = 0; i < m_threadCount; ++i) {
        m_threadContexts[i].Solver.Initialize();
    }
}

void toccata::DecisionTree::SpawnThreads() {
    if (m_threadCount > 1 || ForceMultithreaded) {
        for (int i = 0; i < m_threadCount; ++i) {
            m_threadContexts[i].Thread = new std::thread(&DecisionTree::WorkerThread, this, i);
        }
    }
    else if (m_threadCount == 1) {
        m_threadContexts[0].Thread = nullptr;
    }
}

void toccata::DecisionTree::KillThreads() {
    for (int i = 0; i < m_threadCount; ++i) {
        m_threadContexts[i].Kill = true;
    }

    TriggerThreads();

    if (m_threadCount > 1 || ForceMultithreaded) {
        for (int i = 0; i < m_threadCount; ++i) {
            m_threadContexts[i].Thread->join();
            delete m_threadContexts[i].Thread;

            m_threadContexts[i].Thread = nullptr;
        }
    }
}

void toccata::DecisionTree::Destroy() {
    for (int i = 0; i < m_threadCount; ++i) {
        assert(m_threadContexts[i].Thread == nullptr);
    }

    for (int i = 0; i < m_threadCount; ++i) {
        m_threadContexts[i].Solver.Release();
    }

    Memory::Free(m_threadContexts);

    m_threadContexts = nullptr;
}

void toccata::DecisionTree::Process(int startIndex) {
    for (int i = 0; i < m_threadCount; ++i) {
        m_threadContexts[i].StartIndex = startIndex;
    }

    DistributeWork();
    TriggerThreads();
    WaitForThreads();

    Integrate();
    Prune();
}

void toccata::DecisionTree::DistributeWork() {
    if (m_library != nullptr) {
        int libraryStart = 0;
        const int libraryBars = m_library->GetBarCount();
        const int libraryDelta = (int)std::ceil(libraryBars / (double)m_threadCount);
        for (int i = 0; i < m_threadCount; ++i) {
            int end = libraryStart + libraryDelta - 1;
            if (end >= libraryBars) end = libraryBars - 1;

            m_threadContexts[i].LibraryStart = libraryStart;
            m_threadContexts[i].LibraryEnd = end;

            libraryStart = end + 1;
        }
    }
    else {
        for (int i = 0; i < m_threadCount; ++i) {
            m_threadContexts[i].LibraryStart = 0;
            m_threadContexts[i].LibraryEnd = -1;
        }
    }

    int decisionStart = 0;
    const int decisionCount = GetDecisionCount();
    const int decisionDelta = (int)std::ceil(decisionCount / (double)m_threadCount);
    for (int i = 0; i < m_threadCount; ++i) {
        int end = decisionStart + decisionDelta - 1;
        if (end >= decisionCount) end = decisionCount - 1;

        m_threadContexts[i].DecisionStart = decisionStart;
        m_threadContexts[i].DecisionEnd = end;

        decisionStart = end + 1;
    }
}

void toccata::DecisionTree::TriggerThreads() {
    if (m_threadCount == 1 && !ForceMultithreaded) {
        Work(0, m_threadContexts[0]);
    }
    else {
        for (int i = 0; i < m_threadCount; ++i) {
            std::lock_guard<std::mutex> lk(m_threadContexts[i].Lock);
            m_threadContexts[i].Trigger = true;
            m_threadContexts[i].ConditionVariable.notify_one();
        }
    }
}

void toccata::DecisionTree::WaitForThreads() {
    if (m_threadCount == 1 && !ForceMultithreaded) {
        return;
    }

    for (int i = 0; i < m_threadCount; ++i) {
        std::unique_lock<std::mutex> lk(m_threadContexts[i].Lock);
        m_threadContexts[i].ConditionVariable
            .wait(lk, [this, i] { return m_threadContexts[i].Done; });

        m_threadContexts[i].Done = false;
    }
}

void toccata::DecisionTree::Integrate() {
    for (int i = 0; i < m_threadCount; ++i) {
        std::queue<Decision *> &newDecisions = m_threadContexts[i].NewDecisions;

        while (!newDecisions.empty()) {
            Decision *decision = newDecisions.front(); newDecisions.pop();

            if (!IntegrateDecision(decision)) {
                DestroyDecision(decision);
            }
        }
    }
}

void toccata::DecisionTree::Prune() {
    const int n = GetDecisionCount();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            Decision *a = m_decisions[i];
            Decision *b = m_decisions[j];

            if (a->IsSameAs(b)) {
                if (a->GetDepth() < b->GetDepth()) {
                    a->Flagged = true;
                }
                else if (b->GetDepth() < a->GetDepth()) {
                    b->Flagged = true;
                }
                else if (b->IsBetterFitThan(a)) {
                    a->Flagged = true;
                }
                else if (a->IsBetterFitThan(b)) {
                    b->Flagged = true;
                }
            }
        }
    }

    int newCount = n;
    for (int i = n - 1; i >= 0; --i) {
        Decision *decision = m_decisions[i];
        if (decision->Flagged && decision->Children == 0) {
            if (decision->ParentDecision != nullptr) {
                --decision->ParentDecision->Children;
            }

            DestroyDecision(decision);
            m_decisions[i] = m_decisions[--newCount];
        }
    }

    m_decisions.resize(newCount);
}

bool toccata::DecisionTree::IntegrateDecision(Decision *decision) {
    const int decisionCount = GetDecisionCount();
    for (int i = 0; i < decisionCount; ++i) {
        Decision *currentDecision = m_decisions[i];

        if (decision->IsSameAs(currentDecision)) {
            if (decision->ParentDecision != currentDecision->ParentDecision) {
                continue;
            }
            else {
                if (decision->IsBetterFitThan(currentDecision)) {
                    currentDecision->Placeholder = decision->Placeholder;
                    currentDecision->AverageError = decision->AverageError;
                    currentDecision->MappedNotes = decision->MappedNotes;
                    currentDecision->MappingEnd = decision->MappingEnd;
                    currentDecision->MappingStart = decision->MappingStart;
                    currentDecision->s = decision->s;
                    currentDecision->t = decision->t;
                }

                return false;
            }
        }
    }

    if (decision->ParentDecision != nullptr) {
        ++decision->ParentDecision->Children;
    }

    m_decisions.push_back(decision);

    const int n_next = decision->MatchedBar->GetNextCount();
    for (int i = 0; i < n_next; ++i) {
        toccata::Bar *next = decision->MatchedBar->GetNext(i);
        Decision *placeholder = AllocateDecision();
        placeholder->Placeholder = true;
        placeholder->MatchedBar = next;
        placeholder->ParentDecision = decision;
        placeholder->MappingEnd = decision->MappingStart;

        m_decisions.push_back(placeholder);

        ++decision->Children;
    }

    return true;
}

void toccata::DecisionTree::WorkerThread(int threadId) {
    ThreadContext &context = m_threadContexts[threadId];

    while (!context.Kill) {
        std::unique_lock<std::mutex> lk(context.Lock);
        context.ConditionVariable
            .wait(lk, [this, threadId] { return m_threadContexts[threadId].Trigger; });

        context.Trigger = false;

        Work(threadId, context);

        context.Done = true;

        lk.unlock();
        context.ConditionVariable.notify_one();
    }
}

void toccata::DecisionTree::Work(int threadId, ThreadContext &context) {
    SeedMatch(context.LibraryStart, context.LibraryEnd, threadId);
    PredictionMatch(context.DecisionStart, context.DecisionEnd, threadId);
}

void toccata::DecisionTree::SeedMatch(
    int libraryStart, int libraryEnd, int threadId) 
{
    ThreadContext &context = m_threadContexts[threadId];

    for (int i = libraryStart; i <= libraryEnd; ++i) {
        Bar *bar = m_library->GetBar(i);
        Decision *newDecision = Match(bar, context.StartIndex, context);

        if (newDecision != nullptr) {
            newDecision->ParentDecision = nullptr;
            context.NewDecisions.push(newDecision);
        }
    }
}

void toccata::DecisionTree::PredictionMatch(
    int decisionIndexStart, int decisionIndexEnd, int threadId) 
{
    ThreadContext &context = m_threadContexts[threadId];

    for (int i = decisionIndexStart; i <= decisionIndexEnd; ++i) {
        Decision *decision = m_decisions[i];
        const Bar *bar = decision->MatchedBar;

        int startIndex = context.StartIndex;
        if (startIndex <= decision->MappingEnd) startIndex = decision->MappingEnd + 1;
        if (startIndex >= m_segment->NoteContainer.GetCount()) continue;
        
        int lookaheadCount = bar->GetNextCount();
        for (int j = 0; j < lookaheadCount; ++j) {
            Bar *next = bar->GetNext(j);
            Decision *newDecision = Match(next, startIndex, context);

            if (newDecision != nullptr) {
                newDecision->ParentDecision = decision;
                context.NewDecisions.push(newDecision);
            }
        }
    }
}

toccata::DecisionTree::Decision *toccata::DecisionTree::Match(
    const Bar *reference, int startIndex, ThreadContext &context) 
{
    const int n = reference->GetSegment()->NoteContainer.GetCount();
    const int k = m_segment->NoteContainer.GetCount();

    FullSolver::Result result;
    FullSolver::Request request;
    request.StartIndex = startIndex;
    request.EndIndex = startIndex + (int)std::ceil(n * (1.0 + m_margin)) - 1;
    if (request.EndIndex >= k) request.EndIndex = k - 1;
    request.Reference = reference->GetSegment();
    request.Segment = m_segment;

    bool foundSolution = context.Solver.Solve(request, &result);
    if (!foundSolution) return nullptr;

    Decision *newDecision = AllocateDecision();
    newDecision->AverageError = result.Fit.AverageError;
    newDecision->s = result.s;
    newDecision->t = result.t;
    newDecision->MappingStart = result.Fit.MappingStart;
    newDecision->MappingEnd = result.Fit.MappingEnd;
    newDecision->MappedNotes = result.Fit.MappedNotes;
    newDecision->MatchedBar = reference;

    if (newDecision->AverageError > request.CorrelationThreshold * reference->GetSegment()->Length) {
        int a = 0;
    }

    return newDecision;
}

bool toccata::DecisionTree::Decision::IsSameAs(const Decision *decision) const {
    if (MatchedBar != decision->MatchedBar) return false;
    else if (decision->Placeholder || Placeholder) return true;
    else if (MappingEnd <= decision->MappingEnd && MappingStart >= decision->MappingStart) return true;
    else if (decision->MappingEnd <= MappingEnd && decision->MappingStart >= MappingStart) return true;
    else return false;
}

bool toccata::DecisionTree::Decision::IsBetterFitThan(const Decision *decision) const {
    if (decision->Placeholder && !Placeholder) return true;
    else if (!decision->Placeholder && Placeholder) return false;

    if (MappedNotes > decision->MappedNotes) return true;
    else if (MappedNotes < decision->MappedNotes) return false;

    const int footprint0 = MappingEnd - MappingStart;
    const int footprint1 = decision->MappingEnd - decision->MappingStart;

    if (footprint0 < footprint1) return true;
    else if (footprint1 > footprint0) return false;

    if (AverageError < decision->AverageError) return true;
    else return false;
}

int toccata::DecisionTree::Decision::GetDepth() const {
    if (ParentDecision == nullptr) return 1;
    else return 1 + ParentDecision->GetDepth();
}
