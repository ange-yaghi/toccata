#ifndef TOCCATA_CORE_MIDI_FILE_H
#define TOCCATA_CORE_MIDI_FILE_H

#include "music_segment.h"
#include "music_point.h"
#include "midi_stream.h"

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

    public:
        MidiFile();
        ~MidiFile();

        void Write(const char *fname);
        bool Read(const char *fname, MidiStream *stream);

    protected:
        static void ByteSwap32(UINT32 *data);
        static void ByteSwap24(UINT32 *data);
        static void ByteSwap16(UINT16 *data);

    protected:
        void ReadChunk(MidiChunkHeader *header, MidiStream *stream);
        UINT32 ReadTrackEvent(UINT32 currentTime, MidiStream *stream);

        UINT32 ReadVariableLengthQuantity();

        std::fstream m_file;

        unsigned int m_chunkDataRemaining;
        MusicPoint::Hand m_currentHand = MusicPoint::Hand::Unknown;

    protected:
        // Midi Data
        int m_nTracks;
        int m_format;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MIDI_FILE_H */
