#include "../include/midi_handler.h"

toccata::MidiHandler *toccata::MidiHandler::s_handler = nullptr;

toccata::MidiHandler::MidiHandler() {
    m_buffer.SetTimeSignature(4, 4);

    // 500 milliseconds (ticks) per quarter note
    m_buffer.SetMetricalTimeFormat(500);

    // 120 bpm = 500000 us per quarter note
    m_buffer.SetRawTempo(500000);
}

toccata::MidiHandler::~MidiHandler() {
    /* void */
}

toccata::MidiHandler *toccata::MidiHandler::Get() {
    if (s_handler == nullptr) s_handler = new MidiHandler;

    return s_handler;
}

void toccata::MidiHandler::Extract(MidiStream *targetBuffer) {
    LockBuffer();

    const int noteCount = m_buffer.GetNoteCount();
    for (int i = 0; i < noteCount; ++i) {
        const MidiNote &note = m_buffer.GetNote(i);

        if (note.Valid) {
            targetBuffer->AddNote(note);
        }
    }

    m_buffer.ClearCommittedNotes();

    UnlockBuffer();
}

void toccata::MidiHandler::LockBuffer() {
    m_bufferLock.lock();
}

void toccata::MidiHandler::UnlockBuffer() {
    m_bufferLock.unlock();
}

void toccata::MidiHandler::ProcessEvent(int status, int midiByte1, int midiByte2, unsigned long timestamp) {
    LockBuffer();

    m_buffer.ProcessMidiEvent(status, midiByte1, midiByte2, timestamp);

    UnlockBuffer();
}

void toccata::MidiHandler::ProcessMidiTick(unsigned long timestamp) {
    /* void */
}
