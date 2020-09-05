#include "../include/decision_thread.h"

toccata::DecisionThread::DecisionThread() {
    m_currentIndex = 0;
    m_complete = true;
    m_kill = false;
}

toccata::DecisionThread::~DecisionThread() {
    /* void */
}

void toccata::DecisionThread::Initialize(Library *library, int threadCount, double pulse) {
    m_tree.SetLibrary(library);
    m_tree.SetInputSegment(&m_inputBuffer);
    m_tree.Initialize(threadCount);

    m_complete = false;
    m_kill = false;

    m_inputBuffer.PulseUnit = pulse;
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

    if (m_currentIndex < noteCount) {
        m_tree.Process(m_currentIndex++);
    }
    else m_complete = true;

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
