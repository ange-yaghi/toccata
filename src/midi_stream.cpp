#include "../include/midi_stream.h"

#include <algorithm>

toccata::MidiStream::MidiStream() {
    m_target = nullptr;

    m_negativeSMPTEFormat = 0;
    m_tempo = 0;
    m_ticksPerFrame = 0;
    m_ticksPerQuarterNote = 0;
    m_timeFormat = TimeFormat::TimeCode;
    m_timeSignatureNumerator = 4;
    m_timeSignatureDenominator = 4;
}

toccata::MidiStream::~MidiStream() {
    /* void */
}

void toccata::MidiStream::Sort() {
    std::sort(m_notes.begin(), m_notes.end(),
        [](const MidiNote &a, const MidiNote &b) { return a.Timestamp < b.Timestamp; });
}

void toccata::MidiStream::SetMetricalTimeFormat(unsigned int ticksPerQuarterNote) {
    m_timeFormat = TimeFormat::Metrical;
    m_ticksPerQuarterNote = ticksPerQuarterNote;
}

void toccata::MidiStream::SetTimeCodeTimeFormat(
    unsigned int negativeSmpteFormat, unsigned int ticksPerFrame) 
{
    m_timeFormat = TimeFormat::TimeCode;
    m_negativeSMPTEFormat = negativeSmpteFormat;
    m_ticksPerFrame = ticksPerFrame;
}

unsigned int toccata::MidiStream::GetBarLength() const {
    return ((GetTicksPerQuarterNote() * GetTimeSignatureNumerator()) / 4) * GetTimeSignatureDenominator();
}

double toccata::MidiStream::GetTempo() const {
    return 60.0 / (m_tempo / 1000000.0);
}

void toccata::MidiStream::SetTimeSignature(int numerator, int denominator) {
    m_timeSignatureNumerator = numerator;
    m_timeSignatureDenominator = denominator;
}

void toccata::MidiStream::ClearCommittedNotes() {
    int noteCount = GetNoteCount();
    for (int i = 0; i < noteCount; ++i) {
        if (m_notes[i].Valid) {
            m_notes.erase(m_notes.begin() + i);
            --noteCount;
        }
    }
}

void toccata::MidiStream::ClearEvents() {
    m_events.clear();
}

int toccata::MidiStream::GetEventCount() const {
    return (int)m_events.size();
}

toccata::MidiStream::MidiEvent toccata::MidiStream::GetEvent(int index) const {
    return m_events[index];
}

void toccata::MidiStream::ProcessMidiEvent(
    int status, int byte1, int byte2, unsigned int timestamp, MusicPoint::Hand hand)
{
    if (status == 0x9 || status == 0x8) {
        const int key = byte1;
        const int velocity = byte2;

        if (velocity > 0 && status != 0x8) {
            // Note is pressed
            MidiNote newNote;

            newNote.Timestamp = timestamp;
            newNote.MidiKey = key;
            newNote.Velocity = velocity;
            newNote.AssignedHand = hand;
            newNote.Valid = false;

            AddNote(newNote);

            MidiEvent newEvent;
            newEvent.Event = KeyEvent::On;
            newEvent.Timestamp0 = timestamp;
            newEvent.Timestamp1 = timestamp;
            newEvent.Key = key;

            m_events.push_back(newEvent);
        }
        else {
            // Note is released
            const int lastNote = GetPreviousNote(key, timestamp);

            if (lastNote != -1) {
                m_notes[lastNote].NoteLength = timestamp - m_notes[lastNote].Timestamp;
                m_notes[lastNote].Valid = true;

                MidiEvent newEvent;
                newEvent.Event = KeyEvent::On;
                newEvent.Timestamp0 = m_notes[lastNote].Timestamp;
                newEvent.Timestamp1 = timestamp;
                newEvent.Key = key;

                m_events.push_back(newEvent);
            }
        }
    }
}

int toccata::MidiStream::GetPreviousNote(unsigned int midiNote, unsigned int timestamp) {
    unsigned int smallestDistance = UINT_MAX;
    int closest = -1;
    const int n = (int)m_notes.size();

    for (int i = 0; i < n; ++i) {
        if (m_notes[i].Timestamp < timestamp && m_notes[i].MidiKey == midiNote) {
            const unsigned int diff = timestamp - m_notes[i].Timestamp;

            if (diff < smallestDistance) {
                smallestDistance = diff;
                closest = i;
            }
        }
    }

    return closest;
}
