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

void toccata::DecisionTree::InvalidateAfter(int index) {
    const int n = GetDecisionCount();
    for (int i = 0; i < n; ++i) {
        if (m_decisions[i]->GetEnd() >= index) {
            m_decisions[i]->InvalidateCache();
        }
    }
}

void toccata::DecisionTree::OnNoteChange(int changedNote) {
    const int decisionCount = GetDecisionCount();
    int j = 0;
    for (int i = 0; i < decisionCount; ++i) {
        if (m_decisions[i]->GetEnd() >= changedNote) {
            m_decisions[j] = m_decisions[i];
            m_decisions[j]->Index = j;

            ++j;
        }
    }
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
    const int n = (int)m_decisions.size();

    std::vector<bool> isLeaf(n, true);
    std::vector<int> rightDepth(n, 0);

    for (Decision *decision : m_decisions) {
        GetDepth(decision);

        if (decision->ParentDecision != nullptr) {
            isLeaf[decision->ParentDecision->Index] = false;
        }
    }

    for (const Decision *leaf : m_decisions) {
        if (!isLeaf[leaf->Index]) continue;

        rightDepth[leaf->Index] = 1;

        const Decision *child = leaf;
        const Decision *parent = leaf->ParentDecision;
        while (parent != nullptr) {
            rightDepth[parent->Index] = std::max(
                rightDepth[child->Index] + 1,
                rightDepth[parent->Index]);
            
            child = parent;
            parent = child->ParentDecision;
        }
    }

    int newSize = 0;
    for (int i = 0; i < n; ++i) {
        const Decision *parent = m_decisions[i]->ParentDecision;

        bool deleted = false;
        if (parent != nullptr) {
            if (rightDepth[i] + 1 < rightDepth[parent->Index]) {
                delete m_decisions[i];
                m_decisions[i] = nullptr;

                deleted = true;
            }
        }

        if (!deleted) {
            m_decisions[newSize] = m_decisions[i];

            ++newSize;
        }
    }

    std::vector<std::vector<bool>> overlapping(newSize, std::vector<bool>(newSize, false));
    for (int i = 0; i < newSize; ++i) {
        for (int j = i + 1; j < newSize; ++j) {
            const Decision *d0 = m_decisions[i];
            const Decision *d1 = m_decisions[j];

            const int minNoteCount = std::min(d0->MappedNotes, d1->MappedNotes);
            const int overlap = (int)std::ceil(0.5 * minNoteCount);

            if (d0->GetEnd() >= d1->GetStart() && d0->GetStart() <= d1->GetEnd()) {
                if (d0->Overlapping(d1, overlap)) {
                    overlapping[i][j] = overlapping[j][i] = true;
                }
            }
        }
    }

    int newSize0 = 0;
    for (int i = 0; i < newSize; ++i) {
        bool pruned = false;

        for (int j = 0; j < newSize; ++j) {
            if (i == j) continue;

            const Decision *d0 = m_decisions[i];
            const Decision *d1 = m_decisions[j];

            if (overlapping[i][j] && rightDepth[d0->Index] < rightDepth[d1->Index]) {
                pruned = true;
                break;
            }
        }

        if (pruned) continue;
        m_decisions[newSize0++] = m_decisions[i];
    }

    for (int i = 0; i < newSize0; ++i) {
        m_decisions[i]->Index = i;
    }

    m_decisions.resize(newSize0);
}

int toccata::DecisionTree::GetDepth(Decision *decision) const {
    if (!decision->IsCached()) {
        Decision *bestParent = FindBestParent(decision);

        if (bestParent != nullptr) {
            decision->ParentDecision = bestParent;
            decision->Depth = 1 + GetDepth(bestParent);
        }
        else {
            decision->ParentDecision = nullptr;
            decision->Depth = 1;
        }

        decision->Cached = true;
    }

    return decision->Depth;
}

int toccata::DecisionTree::GetBranchNoteCount(Decision *decision) const {
    if (!decision->IsCached()) {
        GetDepth(decision);
    }

    int noteCount = 0;

    Decision *i = decision;
    while (i->ParentDecision != nullptr) {
        noteCount += i->MappedNotes;
    }

    return noteCount;
}

double toccata::DecisionTree::GetBranchAverageError(Decision *decision) const {
    const int depth = GetDepth(decision);

    double totalError = 0;

    Decision *i = decision;
    while (i->ParentDecision != nullptr) {
        totalError += i->AverageError;
    }

    return totalError / depth;
}

int toccata::DecisionTree::GetBranchStart(Decision *decision) const {
    int start = INT_MAX;

    if (!decision->IsCached()) {
        GetDepth(decision);
    }

    Decision *i = decision;
    while (i->ParentDecision != nullptr) {
        start = std::min(start, i->GetStart());
    }

    return start;
}

int toccata::DecisionTree::GetBranchEnd(Decision *decision) {
    int end = INT_MIN;

    if (!decision->IsCached()) {
        GetDepth(decision);
    }

    Decision *i = decision;
    while (i->ParentDecision != nullptr) {
        end = std::max(end, i->GetStart());
    }

    return end;
}

std::vector<toccata::DecisionTree::MatchedPiece> toccata::DecisionTree::GetPieces() {
    struct Leaf {
        Decision *Decision;
        int Depth;
        int NoteCount;
        double AverageError;
    };

    const int n = GetDecisionCount();

    std::vector<bool> isLeaf(n, true);

    for (const Decision *decision : m_decisions) {
        if (decision->ParentDecision != nullptr) {
            isLeaf[decision->ParentDecision->Index] = false;
        }
    }

    int leafCount = 0;
    for (int i = 0; i < n; ++i) {
        if (isLeaf[i]) ++leafCount;
    }

    std::vector<Leaf> leaves(leafCount);
    int j = 0;
    for (int i = 0; i < n; ++i) {
        if (!isLeaf[i]) continue;
        leaves[j].Decision = m_decisions[i];
        leaves[j].Depth = GetDepth(m_decisions[i]);

        ++j;
    }

    return std::vector<MatchedPiece>();
}

toccata::DecisionTree::Decision *toccata::DecisionTree::FindBestParent(const Decision *decision) const {
    int bestDepth = -1;
    Decision *best = nullptr;

    const int decisions = GetDecisionCount();
    for (int i = 0; i < decisions; ++i) {
        Decision *prev = m_decisions[i];

        if (prev == decision) continue;
        else if (prev->GetEnd() < decision->GetEnd()) {
            if (!prev->MatchedBar->FindNext(decision->MatchedBar, 1) && 
                !decision->MatchedBar->FindNext(prev->MatchedBar, 1)) 
            {
                continue;
            }

            const int depth = GetDepth(prev);

            if (depth > bestDepth) {
                best = prev;
                bestDepth = depth;
            }
        }
    }

    return best;
}

bool toccata::DecisionTree::IntegrateDecision(Decision *decision) {
    const int decisionCount = GetDecisionCount();
    for (int i = 0; i < decisionCount; ++i) {
        Decision *currentDecision = m_decisions[i];

        const int minNoteCount = std::min(currentDecision->MappedNotes, decision->MappedNotes);
        const int overlap = (int)std::ceil(0.5 * minNoteCount);
        
        if (decision->Overlapping(currentDecision, overlap)) {
            if (decision->IsSameAs(currentDecision) ||
                GetDepth(currentDecision) < GetDepth(decision))
            {
                if (decision->IsBetterFitThan(currentDecision)) {
                    currentDecision->AverageError = decision->AverageError;
                    currentDecision->MappedNotes = decision->MappedNotes;
                    currentDecision->Notes = decision->Notes;
                    currentDecision->s = decision->s;
                    currentDecision->t = decision->t;
                    currentDecision->MatchedBar = decision->MatchedBar;

                    InvalidateAfter(currentDecision->GetEnd());
                }
            }

            if (decision->IsSameAs(currentDecision) || 
                GetDepth(currentDecision) != GetDepth(decision)) 
            {
                return false;
            }
        }
    }

    decision->Index = GetDecisionCount();
    m_decisions.push_back(decision);

    InvalidateAfter(decision->GetEnd());

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

toccata::DecisionTree::Decision *toccata::DecisionTree::Match(
    const Bar *reference, int startIndex, ThreadContext &context) 
{
    const int n = reference->GetSegment()->NoteContainer.GetCount();
    const int k = m_segment->NoteContainer.GetCount();

    std::set<int> mappedNotes;

    FullSolver::Result result;
    result.Fit.Target = &mappedNotes;

    FullSolver::Request request;
    request.MissingNoteThreshold = 0.25;
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
    newDecision->Notes = mappedNotes;
    newDecision->MappedNotes = result.Fit.MappedNotes;
    newDecision->MatchedBar = reference;

    return newDecision;
}

bool toccata::DecisionTree::Decision::IsSameAs(const Decision *decision) const {
    if (MatchedBar != decision->MatchedBar) return false;
    else return true;
}

bool toccata::DecisionTree::Decision::IsBetterFitThan(const Decision *decision) const {
    if (MappedNotes > decision->MappedNotes) return true;
    else if (MappedNotes < decision->MappedNotes) return false;

    const int footprint0 = decision->GetFootprint();
    const int footprint1 = decision->GetFootprint();

    if (footprint0 < footprint1) return true;
    else if (footprint1 > footprint0) return false;

    if (AverageError < decision->AverageError) return true;
    else return false;
}

bool toccata::DecisionTree::Decision::IsCached() const {
    if (!Cached) return false;
    
    if (ParentDecision != nullptr) {
        return ParentDecision->IsCached();
    }
    else return true;
}

void toccata::DecisionTree::Decision::InvalidateCache() {
    Cached = false;
    ParentDecision = nullptr;
    Depth = 0;
}

int toccata::DecisionTree::Decision::GetFootprint() const {
    return GetEnd() - GetStart() + 1;
}

int toccata::DecisionTree::Decision::GetEnd() const {
    return *(Notes.rbegin());
}

int toccata::DecisionTree::Decision::GetStart() const {
    return *Notes.begin();
}

bool toccata::DecisionTree::Decision::Overlapping(const Decision *decision, int overlap) const {
    if (decision->GetEnd() < GetStart()) return false;
    if (decision->GetStart() > GetEnd()) return false;
    
    if (decision->MappedNotes < MappedNotes) return decision->Overlapping(this, overlap);
    
    int sharedNotes = 0;
    for (int n0 : Notes) {
        if (decision->Notes.count(n0) > 0) {
            if (++sharedNotes >= overlap) return true;
        }
    }

    return false;
}
