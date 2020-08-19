#ifndef TOCCATA_CORE_MIDI_FILE_H
#define TOCCATA_CORE_MIDI_FILE_H

#include "music_segment.h"
#include "music_point.h"

#include <fstream>
#include <vector>

#include <Windows.h>

namespace toccata {

    class MidiFile {
    public:
        struct MidiChunkHeader {
            BYTE Type[4];
            UINT32 Length;
        };

        struct alignas(2) MidiHeaderChunkData {
            UINT16 Format;
            UINT16 Ntrks;
            UINT16 Division;
        };

        struct alignas(2) MidiMTrkEventHeader {
            UINT16 DeltaTime;
        };

        enum class TimeFormat {
            Metrical,
            TimeCode
        };

        struct MidiNote {
            BYTE MidiKey;
            BYTE Velocity;

            int NoteLength;
            UINT32 Timestamp;

            MusicPoint::Hand AssignedHand;
        };

    public:
        MidiFile();
        ~MidiFile();

        void Write(const char *fname);
        bool Read(const char *fname);

        static void ByteSwap32(UINT32 *data);
        static void ByteSwap24(UINT32 *data);
        static void ByteSwap16(UINT16 *data);

        double GetTempo() const;
        int GetTimeSignatureNumerator() const { return m_timeSignatureNumerator; }
        int GetTimeSignatureDenominator() const { return m_timeSignatureDenominator; }

        const MidiNote &GetNote(int i) const { return m_notes[i]; }
        int GetNoteCount() const { return (int)m_notes.size(); }

        void AddNote(const MidiNote &note) { m_notes.push_back(note); }

        bool IsMetrical() const { return m_timeFormat == TimeFormat::Metrical; }
        int GetTicksPerQuarterNote() const { return m_ticksPerQuarterNote; }

    protected:
        int GetPreviousNote(BYTE midiNote, UINT32 timestamp);

    protected:
        void ReadChunk(MidiChunkHeader *header);
        UINT32 ReadTrackEvent(UINT32 currentTime);

        UINT32 ReadVariableLengthQuantity();

        std::fstream m_file;

        std::vector<MidiNote> m_notes;

        // Tempo information
        TimeFormat m_timeFormat;
        int m_ticksPerQuarterNote;
        int m_negativeSMPTEFormat;
        int m_ticksPerFrame;
        int m_tempo;

        int m_timeSignatureNumerator;
        int m_timeSignatureDenominator;

        unsigned int m_chunkDataRemaining;
        MusicPoint::Hand m_currentHand = MusicPoint::Hand::Unknown;

    protected:
        // Midi Data
        int m_nTracks;
        int m_format;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MIDI_FILE_H */
