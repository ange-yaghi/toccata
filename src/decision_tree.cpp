#include "../include/decision_tree.h"

#include "../include/memory.h"
#include "../include/transform.h"

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
            DeleteDecision(m_decisions[i]);
            delete m_decisions[i];
        }
        else {
            m_decisions[j] = m_decisions[i];
            m_decisions[j]->Index = j;

            ++j;
        }
    }

    m_decisions.resize(j);
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

void toccata::DecisionTree::UpdateDecision(Decision *target, Decision *source) {
    target->AverageError = source->AverageError;
    target->MappedNotes = source->MappedNotes;
    target->Notes = source->Notes;
    target->s = source->s;
    target->t = source->t;
    target->MatchedBar = source->MatchedBar;
    target->Singular = source->Singular;

    InvalidateAfter(target->GetEnd());
    UpdateOverlapMatrix(target);
}

void toccata::DecisionTree::DeleteDecision(Decision *decision) {
    CleanOverlapMatrix(decision);

    for (Decision *d : m_decisions) {
        if (d->ParentDecision == decision) {
            d->InvalidateCache();
        }
    }
}

void toccata::DecisionTree::UpdateOverlapMatrix(Decision *d0) {
    CleanOverlapMatrix(d0);

    for (Decision *d1 : m_decisions) {
        if (d0 == d1) continue;

        const int minNoteCount = std::min(d0->MappedNotes, d1->MappedNotes);
        const int overlap = (int)std::ceil(0.5 * minNoteCount);

        if (d0->Overlapping(d1, overlap)) {
            d0->OverlappingDecisions.push_back(d1);
            d1->OverlappingDecisions.push_back(d0);
        }
    }
}

void toccata::DecisionTree::CleanOverlapMatrix(Decision *decision) {
    for (Decision *overlap : decision->OverlappingDecisions) {
        for (auto i = overlap->OverlappingDecisions.begin();
            i != overlap->OverlappingDecisions.end(); ++i) 
        {
            if (*i == decision) {
                overlap->OverlappingDecisions.erase(i);
                break;
            }
        }
    }

    decision->OverlappingDecisions.clear();
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
    while (i != nullptr) {
        noteCount += i->MappedNotes;
        i = i->ParentDecision;
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

void toccata::DecisionTree::Clear() {
    for (Decision *decision : m_decisions) {
        delete decision;
    }

    m_decisions.clear();
}

std::vector<toccata::DecisionTree::MatchedPiece> toccata::DecisionTree::GetPieces() {
    const int n = GetDecisionCount();

    std::vector<bool> isLeaf(n, true);

    for (Decision *decision : m_decisions) {
        GetDepth(decision);

        if (decision->ParentDecision != nullptr) {
            isLeaf[decision->ParentDecision->Index] = false;
        }
    }

    int leafCount = 0;
    for (int i = 0; i < n; ++i) {
        if (isLeaf[i]) ++leafCount;
    }

    std::vector<MatchedPiece> results;

    for (int i = 0; i < n; ++i) {
        if (!isLeaf[i]) continue;

        Decision *decision = m_decisions[i];
        float s_avg = 0.0;
        int s_samples = 0;
        while (decision != nullptr) {
            if (!decision->Singular) {
                s_avg += decision->s;
                ++s_samples;
            }

            decision = decision->ParentDecision;
        }

        s_avg /= s_samples;

        MatchedPiece newPiece;
        newPiece.Start = INT_MAX;
        newPiece.End = INT_MIN;
        newPiece.MatchedNotes = GetBranchNoteCount(m_decisions[i]);

        decision = m_decisions[i];
        while (decision != nullptr) {
            MatchedBar bar;
            bar.MatchedBar = decision->MatchedBar;
            bar.Start = decision->GetStart();
            bar.End = decision->GetEnd();

            if (decision->Singular && s_samples > 0) {
                bar.s = s_avg;
                bar.t = decision->t * s_avg;
            }
            else {
                bar.s = decision->s;
                bar.t = decision->t;
            }

            newPiece.Start = std::min(bar.Start, newPiece.Start);
            newPiece.End = std::max(bar.End, newPiece.End);

            newPiece.Bars.push_back(bar);

            decision = decision->ParentDecision;
        }

        std::reverse(newPiece.Bars.begin(), newPiece.Bars.end());

        results.push_back(newPiece);
    }

    std::sort(results.begin(), results.end(),
        [](MatchedPiece &a, MatchedPiece &b) {
            if (a.MatchedNotes == b.MatchedNotes) {
                return (a.End - a.Start + 1) < (b.End - b.Start + 1);
            }
            else return a.MatchedNotes > b.MatchedNotes;
        });

    const int pieceCount = (int)results.size();
    std::vector<bool> filtered(pieceCount, false);
    for (int i = 0; i < pieceCount; ++i) {
        if (filtered[i]) continue;

        const MatchedPiece &piece0 = results[i];
        const int length0 = piece0.End - piece0.Start + 1;

        for (int j = i + 1; j < pieceCount; ++j) {
            const MatchedPiece &piece1 = results[j];
            const int length1 = piece1.End - piece1.Start + 1;

            const int start = std::max(piece0.Start, piece1.Start);
            const int end = std::min(piece0.End, piece1.End);

            const int overlap = std::max(0, end - start + 1);
            const int overlapThreshold = std::ceil(0.25 * std::min(length0, length1));

            if (overlap >= overlapThreshold) {
                filtered[j] = true;
            }
        }
    }

    int newSize = 0;
    for (int i = 0; i < pieceCount; ++i) {
        if (filtered[i]) continue;

        results[newSize++] = results[i];
    }

    results.resize(newSize);

    std::sort(results.begin(), results.end(),
        [](MatchedPiece &a, MatchedPiece &b) {
            return a.End < b.End;
        });

    return results;
}

toccata::DecisionTree::Decision *toccata::DecisionTree::FindBestParent(const Decision *decision) const {
    int bestNoteCount = -1;
    Decision *best = nullptr;

    const int decisions = GetDecisionCount();
    for (int i = 0; i < decisions; ++i) {
        Decision *prev = m_decisions[i];

        if (prev == decision) continue;
        else if (prev->GetEnd() < decision->GetEnd()) {
            bool isOverlapping = false;
            for (Decision *overlapping : prev->OverlappingDecisions) {
                if (overlapping == decision) {
                    isOverlapping = true;
                    break;
                }
            }

            if (isOverlapping) continue;

            const Bar::SearchResult next = prev->MatchedBar->FindNext(decision->MatchedBar, 1);
            if (next.Offset == -1) {
                continue;
            }

            const double prevLength = prev->MatchedBar->GetSegment()->Length;
            const double length = decision->MatchedBar->GetSegment()->Length;
            const double decisionStart = Transform::inv_f(0.0, decision->s, decision->t);
            const double prevEnd = Transform::inv_f(prevLength, prev->s, prev->t);
            const double trueDistance = Transform::f(decisionStart - prevEnd, decision->s, 0.0);

            if (trueDistance > next.Distance + length * 0.5) continue;

            GetDepth(prev);
            const int noteCount = GetBranchNoteCount(prev);
            if (best == nullptr || noteCount > bestNoteCount || prev->GetEnd() > best->GetEnd()) {
                best = prev;
                bestNoteCount = noteCount;
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
            if (decision->IsSameAs(currentDecision)) {
                if (decision->IsBetterFitThan(currentDecision)) {
                    UpdateDecision(currentDecision, decision);
                }

                return false;
            }
        }
    }

    decision->Index = GetDecisionCount();
    m_decisions.push_back(decision);

    InvalidateAfter(decision->GetEnd());
    UpdateOverlapMatrix(decision);

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
    newDecision->Singular = result.Singular;

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
