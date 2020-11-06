#include "../include/decision_thread.h"

#include <chrono>

toccata::DecisionThread::DecisionThread() {
    m_currentIndex = 0;
    m_complete = true;
    m_kill = false;

    m_peakIndex = 0;
    m_peakIndexReset = true;

    m_peakLatency = 0.0;
    m_peakLatencyReset = true;

    m_peakTargetIndex = 0;
    m_peakTargetIndexReset = true;
}

toccata::DecisionThread::~DecisionThread() {
    /* void */
}

void toccata::DecisionThread::Initialize(Library *library, int threadCount, double pulseUnit, double pulseRate) {
    m_tree.SetLibrary(library);
    m_tree.SetInputSegment(&m_inputBuffer);
    m_tree.Initialize(threadCount);

    m_complete = false;
    m_kill = false;

    m_inputBuffer.PulseUnit = pulseUnit;
    m_inputBuffer.PulseRate = pulseRate;
}

void toccata::DecisionThread::StartThreads() {
    m_tree.SpawnThreads();

    m_thread = std::thread(&DecisionThread::RunThread, this);
}

void toccata::DecisionThread::KillThreads() {
    m_kill = true;
    m_thread.join();

    m_tree.KillThreads();
}

void toccata::DecisionThread::Destroy() {
    m_tree.Destroy();
}

void toccata::DecisionThread::DoIteration() {
    m_bufferLock.lock();

    const int noteCount = m_inputBuffer.NoteContainer.GetCount();

    auto start = std::chrono::steady_clock::now();    

    if (m_currentIndex < noteCount) {
        m_tree.Process(m_currentIndex++);
    }
    else m_complete = true;

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    RecordLatency(elapsed.count());
    RecordIndex(m_currentIndex);
    RecordTargetIndex(noteCount);

    m_bufferLock.unlock();
}

void toccata::DecisionThread::RunThread() {
    while (!m_kill) {
        DoIteration();
    }
}

void toccata::DecisionThread::AddNote(const MusicPoint &point) {
    m_bufferLock.lock();

    const int index = m_inputBuffer.NoteContainer.AddPoint(point);
    m_tree.OnNoteChange(index);

    m_currentIndex = std::min(
        m_currentIndex,
        std::max(0, index - 128));
    m_complete = false;

    m_bufferLock.unlock();
}

void toccata::DecisionThread::Clear() {
    m_bufferLock.lock();

    m_tree.Clear();

    m_bufferLock.unlock();
}

std::vector<toccata::DecisionTree::MatchedPiece> toccata::DecisionThread::GetPieces() {    
    m_bufferLock.lock();

    std::vector<toccata::DecisionTree::MatchedPiece> result = m_tree.GetPieces();

    m_bufferLock.unlock();

    return result;
}

void toccata::DecisionThread::RecordIndex(int index) {
    if (m_peakIndexReset) {
        m_peakIndex = index;
        m_peakIndexReset = false;
    }
    else {
        m_peakIndex = std::min(index, m_peakIndex);
    }
}

int toccata::DecisionThread::ReadPeakIndex() {
    int peakIndex;

    m_bufferLock.lock();

    m_peakIndexReset = true;
    peakIndex = m_peakIndex;

    m_bufferLock.unlock();

    return peakIndex;
}

void toccata::DecisionThread::RecordTargetIndex(int index) {
    if (m_peakTargetIndexReset) {
        m_peakTargetIndex = index;
        m_peakTargetIndexReset = false;
    }
    else {
        m_peakTargetIndex = std::max(m_peakTargetIndex, index);
    }
}

int toccata::DecisionThread::ReadPeakTargetIndex() {
    int peakTargetIndex;

    m_bufferLock.lock();

    m_peakTargetIndexReset = true;
    peakTargetIndex = m_peakTargetIndex;

    m_bufferLock.unlock();

    return m_peakTargetIndex;
}

void toccata::DecisionThread::RecordLatency(double latency) {
    if (m_peakLatencyReset) {
        m_peakLatency = latency;
        m_peakLatencyReset = false;
    }
    else {
        m_peakLatency = std::max(m_peakLatency, latency);
    }
}

double toccata::DecisionThread::ReadPeakLatency() {
    double peakLatency;

    m_bufferLock.lock();

    m_peakLatencyReset = true;
    peakLatency = m_peakLatency;

    m_bufferLock.unlock();

    return peakLatency;
}
