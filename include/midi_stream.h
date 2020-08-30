#ifndef TOCCATA_CORE_MIDI_STREAM_H
#define TOCCATA_CORE_MIDI_STREAM_H

#include "midi_note.h"
#include "music_segment.h"

#include <vector>

namespace toccata {

    class MidiStream {
    public:
        enum class KeyEvent {
            On,
            Length
        };

        struct MidiEvent {
            KeyEvent Event;
            int Key;
            unsigned int Timestamp0;
            unsigned int Timestamp1;
        };

        enum class TimeFormat {
            Metrical,
            TimeCode
        };

    public:
        MidiStream();
        ~MidiStream();

        double GetTempo() const;
        void SetTimeSignature(int numerator, int denominator);
        int GetTimeSignatureNumerator() const { return m_timeSignatureNumerator; }
        int GetTimeSignatureDenominator() const { return m_timeSignatureDenominator; }

        void ClearCommittedNotes();
        void AddNote(const MidiNote &note) { m_notes.push_back(note); }
        const MidiNote &GetNote(int i) const { return m_notes[i]; }
        int GetNoteCount() const { return (int)m_notes.size(); }

        void ClearEvents();
        void AddEvent(const MidiEvent &newEvent) { m_events.push_back(newEvent); }
        int GetEventCount() const;
        MidiEvent GetEvent(int index) const;

        bool IsMetrical() const { return m_timeFormat == TimeFormat::Metrical; }
        int GetTicksPerQuarterNote() const { return m_ticksPerQuarterNote; }
        
        unsigned int GetBarLength() const;

        void Sort();

        void SetMetricalTimeFormat(unsigned int ticksPerQuarterNote);
        void SetTimeCodeTimeFormat(unsigned int negativeSmpteFormat, unsigned int ticksPerFrame);

        TimeFormat GetTimeFormat() const { return m_timeFormat; }

        void SetRawTempo(unsigned int tempo) { m_tempo = tempo; }
        unsigned int GetRawTempo() const { return m_tempo; }

        void ProcessMidiEvent(
            int status, int byte1, int byte2, 
            unsigned int timestamp, MusicPoint::Hand hand = MusicPoint::Hand::Unknown);

        void SetTarget(MusicSegment *target) { m_target = target; }
        MusicSegment *GetTarget() const { return m_target; }

    protected:
        int GetPreviousNote(unsigned int midiNote, unsigned int timestamp);

        // Tempo information
        TimeFormat m_timeFormat;
        unsigned int m_ticksPerQuarterNote;
        unsigned int m_negativeSMPTEFormat;
        unsigned int m_ticksPerFrame;
        unsigned int m_tempo;

        int m_timeSignatureNumerator;
        int m_timeSignatureDenominator;

    protected:
        MusicSegment *m_target;

    protected:
        std::vector<MidiNote> m_notes;
        std::vector<MidiEvent> m_events;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MIDI_STREAM_H */
