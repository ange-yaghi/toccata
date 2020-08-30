#include "../include/midi_handler.h"

toccata::MidiHandler *toccata::MidiHandler::s_handler = nullptr;

toccata::MidiHandler::MidiHandler() {
    m_buffer.SetTimeSignature(4, 4);

    // 500 milliseconds (ticks) per quarter note
    m_buffer.SetMetricalTimeFormat(500);

    // 120 bpm = 500000 us per quarter note
    m_buffer.SetRawTempo(500000);

    m_timestampOffset = 0;
    m_lastTimestamp = 0;
}

toccata::MidiHandler::~MidiHandler() {
    /* void */
}

toccata::MidiHandler *toccata::MidiHandler::Get() {
    if (s_handler == nullptr) s_handler = new MidiHandler;

    return s_handler;
}

void toccata::MidiHandler::Extract(MidiStream *targetBuffer) {
    m_bufferLock.lock();

    const int noteCount = m_buffer.GetNoteCount();
    for (int i = 0; i < noteCount; ++i) {
        const MidiNote &note = m_buffer.GetNote(i);

        if (note.Valid) {
            targetBuffer->AddNote(note);
        }
    }

    const int eventCount = m_buffer.GetEventCount();
    for (int i = 0; i < eventCount; ++i) {
        targetBuffer->AddEvent(m_buffer.GetEvent(i));
    }

    m_buffer.ClearEvents();
    m_buffer.ClearCommittedNotes();

    m_bufferLock.unlock();
}

void toccata::MidiHandler::ProcessEvent(int status, int midiByte1, int midiByte2, unsigned long timestamp) {
    m_bufferLock.lock();

    m_lastTimestamp = timestamp;

    m_buffer.ProcessMidiEvent(status, midiByte1, midiByte2, timestamp + m_timestampOffset);

    m_bufferLock.unlock();
}

void toccata::MidiHandler::ProcessMidiTick(unsigned long timestamp) {
    /* void */
}

void toccata::MidiHandler::AlignTimestampOffset() {
    m_timestampOffset = m_lastTimestamp;
}
