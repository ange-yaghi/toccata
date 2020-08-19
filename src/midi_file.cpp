#include "../include/midi_file.h"

#include <algorithm>

toccata::MidiFile::MidiFile() {
    m_timeFormat = TimeFormat::Metrical;
    m_ticksPerQuarterNote = 0;
    m_negativeSMPTEFormat = 0;
    m_ticksPerFrame = 0;
    m_tempo = 100;

    m_format = 0;
    m_nTracks = 0;

    m_timeSignatureNumerator = 4;
    m_timeSignatureDenominator = 4;

    m_chunkDataRemaining = 0;
}

toccata::MidiFile::~MidiFile() {
    /* void */
}

void toccata::MidiFile::Write(const char *fname) {
    /* TODO */
}

bool toccata::MidiFile::Read(const char *fname) {
    m_file.open(fname, std::ios::binary | std::ios::in);

    if (!m_file.is_open()) {
        return false;
    }

    while (true) {
        MidiChunkHeader chunkHeader;

        m_file.read((char *)&chunkHeader, sizeof(MidiChunkHeader));
        if (m_file.eof()) break;

        ByteSwap32(&chunkHeader.Length);

        m_chunkDataRemaining = chunkHeader.Length;

        ReadChunk(&chunkHeader);
    }

    m_file.close();

    std::sort(m_notes.begin(), m_notes.end(),
        [](const MidiNote &a, const MidiNote &b) { return a.Timestamp < b.Timestamp; });

    return true;
}

void toccata::MidiFile::ByteSwap32(UINT32 *data) {
    UINT32 sdata = *data;

    *data =
        ((sdata >> 24) & 0x000000ff) |
        ((sdata << 8) & 0x00ff0000) |
        ((sdata >> 8) & 0x0000ff00) |
        ((sdata << 24) & 0xff000000);
}

void toccata::MidiFile::ByteSwap24(UINT32 *data) {
    UINT32 sdata = *data;

    *data =
        ((sdata >> 16) & 0x0000ff) |
        ((sdata) & 0x00ff00) |
        ((sdata << 16) & 0xff0000);
}

void toccata::MidiFile::ByteSwap16(UINT16 *data) {
    UINT32 sdata = *data;

    *data =
        ((sdata >> 8) & 0x00ff) |
        ((sdata << 8) & 0xff00);
}

double toccata::MidiFile::GetTempo() const {
    return 60.0 / (m_tempo / 1000000.0);
}

int toccata::MidiFile::GetPreviousNote(BYTE midiNote, UINT32 timestamp) {
    UINT32 smallestDistance = INT_MAX;
    int closest = -1;
    int n = (int)m_notes.size();

    for (int i = 0; i < n; ++i) {
        if (m_notes[i].Timestamp < timestamp) {
            UINT32 diff = timestamp - m_notes[i].Timestamp;

            if (diff < smallestDistance) {
                smallestDistance = diff;
                closest = i;
            }
        }
    }

    return closest;
}

void toccata::MidiFile::ReadChunk(MidiChunkHeader *header) {
    if (header->Type[0] == 'M' &&
        header->Type[1] == 'T' &&
        header->Type[2] == 'h' &&
        header->Type[3] == 'd') 
    {
        MidiHeaderChunkData chunkData;

        m_file.read((char *)&chunkData, sizeof(MidiHeaderChunkData));

        m_chunkDataRemaining -= sizeof(MidiHeaderChunkData);

        ByteSwap16(&chunkData.Division);
        ByteSwap16(&chunkData.Format);
        ByteSwap16(&chunkData.Ntrks);

        m_format = chunkData.Format;

        // Get bit 15
        int timeFormat = chunkData.Division & 0x8000;

        if (timeFormat == 0) {
            // Metrical time
            m_timeFormat = TimeFormat::Metrical;
            m_ticksPerQuarterNote = chunkData.Division & 0x7FFF;

        }
        else {
            // Time-code time
            m_timeFormat = TimeFormat::TimeCode;
            m_negativeSMPTEFormat = (chunkData.Division & 0x7FFF) >> 8;
            m_ticksPerFrame = (chunkData.Division & 0x00FF);
        }
    }
    else if (header->Type[0] == 'M' &&
        header->Type[1] == 'T' &&
        header->Type[2] == 'r' &&
        header->Type[3] == 'k') 
    {
        // Read a track chunk

        // By default track data is read into the right hand
        // unless specified otherwise
        m_currentHand = MusicPoint::Hand::RightHand;

        UINT32 currentTime = 0;

        // Read track events until there
        // are no bytes remaining
        while (m_chunkDataRemaining > 0) {
            currentTime += ReadTrackEvent(currentTime);
        }
    }
}

UINT32 toccata::MidiFile::ReadTrackEvent(UINT32 currentTime) {
    UINT32 deltaTime = ReadVariableLengthQuantity();
    currentTime += deltaTime;

    BYTE type;

    m_file.read((char *)&type, sizeof(BYTE));
    m_chunkDataRemaining -= sizeof(BYTE);

    if (type == 0xF0) {
        // System exclusive message
    }
    else if (type == 0xF7) {
        // System exclusive (resume)
    }
    else if (type == 0xFF) {
        // Meta data
        BYTE metaType;
        UINT32 length;

        m_file.read((char *)&metaType, sizeof(BYTE));
        m_chunkDataRemaining -= sizeof(BYTE);

        length = ReadVariableLengthQuantity();

        if (metaType == 0x01) {
            // Text event
            char text[256];

            m_file.read(text, length);
            text[length] = '\0';

            m_chunkDataRemaining -= length;
        }
        else if (metaType == 0x03) {
            // Track name
            char text[256];

            m_file.read(text, length);
            text[length] = '\0';

            if (strcmp(text, "Left-hand:") == 0) {
                m_currentHand = MusicPoint::Hand::LeftHand;
            }
            else {
                m_currentHand = MusicPoint::Hand::RightHand;
            }

            m_chunkDataRemaining -= length;
        }
        else if (metaType == 0x51) {
            // Change tempo

            UINT32 tempo = 0;
            m_file.read((char *)&tempo, sizeof(BYTE) * 3);

            ByteSwap24(&tempo);

            m_tempo = tempo;
            m_chunkDataRemaining -= sizeof(BYTE) * 3;
        }
        else if (metaType == 0x2F) {
            // End of track
        }
        else if (metaType == 0x58) {
            // Time Signature

            UINT8 ts[4];
            m_file.read((char *)ts, sizeof(BYTE) * 4);

            UINT8 numerator;
            UINT8 denominator;
            UINT8 clocksPerTick;
            UINT8 notated32ndNotesPerBeat;

            numerator = ts[0];
            denominator = ts[1];
            clocksPerTick = ts[2];
            notated32ndNotesPerBeat = ts[3];

            m_timeSignatureNumerator = (int)numerator;
            m_timeSignatureDenominator = 2 << ((int)denominator - 1);

            m_chunkDataRemaining -= sizeof(BYTE) * 4;
        }
        else {
            m_file.ignore(length);
            m_chunkDataRemaining -= length;
        }
    }
    else {
        if (type >= 192 && type <= 223) {
            /* void */
        }

        // Normal MIDI event

        BYTE midiStatus;
        BYTE byte1;
        BYTE byte2;

        midiStatus = type;

        // Data 1
        m_file.read((char *)&byte1, sizeof(BYTE));

        // Data 2
        m_file.read((char *)&byte2, sizeof(BYTE));

        m_chunkDataRemaining -= sizeof(BYTE) * 2;

        if (midiStatus == 0x90 || midiStatus == 0x91) {
            BYTE key = byte1;
            BYTE velocity = byte2;

            if (velocity > 0) {
                // Note is pressed
                MidiNote newNote;

                newNote.Timestamp = currentTime;
                newNote.MidiKey = key;
                newNote.Velocity = velocity;
                newNote.AssignedHand = m_currentHand;

                m_notes.push_back(newNote);
            }
            else {
                // Note is released
                int lastNote = GetPreviousNote(key, currentTime);

                if (lastNote != -1) {
                    m_notes[lastNote].NoteLength = currentTime - m_notes[lastNote].Timestamp;
                }
            }
        }
        else {
            /* void */
        }
    }

    return deltaTime;
}

UINT32 toccata::MidiFile::ReadVariableLengthQuantity() {
    BYTE data;
    UINT32 vlq = 0x00000000;

    int nBytes = 0;
    bool done = false;

    while (!done) {
        m_file.read((char *)&data, sizeof(BYTE));
        m_chunkDataRemaining -= sizeof(BYTE);

        if ((data & 0x80) == 0x00) {
            // Variable length quantity is done
            done = true;
        }

        vlq <<= 7;
        vlq |= data & 0x7F;

        nBytes++;
    }

    return vlq;
}
