#include "../include/midi.h"
#include "../include/midi_output.h"

#include <fstream>

toccata::MidiNote::MidiNote() {
    m_parent = nullptr;
    m_velocity = 0;

    m_timeStamp = 0;
    m_realTime = 0;
    m_midiKey = 0;
    m_noteLength = 0;

    m_valid = true;

    m_classType = NoteClass::Standard;
}

toccata::MidiNote::MidiNote(NoteClass classType) {
    m_parent = nullptr;
    m_velocity = 0;

    m_timeStamp = 0;
    m_realTime = 0;
    m_midiKey = 0;
    m_noteLength = 0;

    m_valid = true;

    m_classType = classType;
}

toccata::MidiNote::~MidiNote() {
    /* void */
}

void toccata::MidiNote::GetNaturalNotation(char *letter, bool *accidental, int *octave) {
    int delta = m_midiKey - 12;

    *octave = delta / 12;

    int natural = (m_midiKey - 21) % 12;

    int c_offset = delta % 12;

    if (c_offset == 1 ||
        c_offset == 3 ||
        c_offset == 6 ||
        c_offset == 8 ||
        c_offset == 10) 
    {
        *accidental = true;
        c_offset -= 1;
        natural -= 1;
    }
    else *accidental = false;

    if (c_offset > 10) c_offset -= 5;
    else if (c_offset > 8) c_offset -= 4;
    else if (c_offset > 6) c_offset -= 3;
    else if (c_offset > 3) c_offset -= 2;
    else if (c_offset > 1) c_offset -= 1;

    *letter = 'A' + (2 + c_offset) % 7;
}

void toccata::MidiNote::PrintDebug(int tempo) {
    char letter;
    bool accidental;
    int octave;

    GetNaturalNotation(&letter, &accidental, &octave);

    if (tempo == 0) {
        if (accidental) {
            printf("%c#%d | %d\n", letter, octave, m_parent->GetSegment()->ConvertDeltaTimeToNote(m_noteLength));
        }
        else {
            printf("%c%d | %d\n", letter, octave, m_parent->GetSegment()->ConvertDeltaTimeToNote(m_noteLength));
        }
    }
    else {
        if (accidental) {
            printf("%c#%d | %d : %d\n", letter, octave, m_parent->GetSegment()->ConvertDeltaTimeToMilliseconds(m_timeStamp, tempo),
                m_parent->GetSegment()->ConvertDeltaTimeToMilliseconds(m_noteLength, tempo));
        }
        else {
            printf("%c%d | %d : %d\n", letter, octave, m_parent->GetSegment()->ConvertDeltaTimeToMilliseconds(m_timeStamp, tempo),
                m_parent->GetSegment()->ConvertDeltaTimeToMilliseconds(m_noteLength, tempo));
        }
    }
}

toccata::MidiPianoSegment *toccata::MidiNote::GetSegment() {
    return m_parent->GetSegment();
}

int toccata::MidiNote::GetTimeStampMilliseconds(int offset, int tempo) const {
    int timeStamp_ms = m_parent->GetSegment()->ConvertDeltaTimeToMilliseconds(m_timeStamp + offset, tempo);

    return timeStamp_ms;
}

void toccata::MidiNote::CopyTo(MidiNote *midiNote) const {
    midiNote->m_midiKey = m_midiKey;
    midiNote->m_timeStamp = m_timeStamp;
    midiNote->m_valid = m_valid;
    midiNote->m_noteLength = m_noteLength;
    midiNote->m_realTime = m_realTime;
    midiNote->m_velocity = m_velocity;
}

toccata::MidiAnalysisNote::MidiAnalysisNote() : MidiNote() {
    m_errorType = NoteErrorType::None;
    m_timingError = 0.0;
    m_realTimeError = 0.0;
}

toccata::MidiAnalysisNote::~MidiAnalysisNote() {
    /* void */
}

void toccata::MidiAnalysisNote::CopyTo(MidiNote *midiNote) const {
    MidiNote::CopyTo(midiNote);

    if (midiNote->GetClassType() == NoteClass::Analysis) {
        MidiAnalysisNote *analysisNote = static_cast<MidiAnalysisNote *>(midiNote);
        analysisNote->m_errorType = m_errorType;
        analysisNote->m_timingError = m_timingError;
        analysisNote->m_realTimeError = m_realTimeError;
    }
}

toccata::MidiTrack::MidiTrack() {
    m_parent = nullptr;
}

toccata::MidiTrack::~MidiTrack() {
    /* void */
}

toccata::MidiNote *toccata::MidiTrack::AddNote() {
    MidiNote *newNote = m_notes.New();

    newNote->SetParent(this);

    return newNote;
}

toccata::MidiAnalysisNote *toccata::MidiTrack::AddAnalysisNote() {
    MidiAnalysisNote *newNote = m_notes.NewGeneric<MidiAnalysisNote>();

    newNote->SetParent(this);

    return newNote;
}

void toccata::MidiTrack::TransferNote(MidiNote *note, MidiTrack *target) {
    m_notes.Delete(note->GetIndex(), false, NULL, true);

    note->SetParent(target);
    target->m_notes.Add(note);
}

void toccata::MidiTrack::DeleteNote(MidiNote *note) {
    m_notes.Delete(note->GetIndex(), true, NULL, true);
}

void toccata::MidiTrack::PrintDebug(int tempo) {
    int nNotes = m_notes.GetNumObjects();

    for (int i = 0; i < nNotes; i++) {
        m_notes.Get(i)->PrintDebug(tempo);
    }
}

toccata::MidiNote *toccata::MidiTrack::FindLastNote(int midiKey, int timeStamp) {
    int nNotes = m_notes.GetNumObjects();

    for (int i = nNotes - 1; i >= 0; i--) {
        if (m_notes.Get(i)->m_midiKey == midiKey) {
            if (m_notes.Get(i)->m_timeStamp <= timeStamp) {
                return m_notes.Get(i);
            }
        }
    }

    return nullptr;
}

toccata::MidiNote *toccata::MidiTrack::FindClosestNote(int midiKey, int timeStamp, int timeThreshold, int startIndex) {
    int nNotes = m_notes.GetNumObjects();

    if (nNotes == 0) return nullptr;

    MidiNote *closestNote = nullptr;
    int refKeyDiff = -1;
    int refTimeDiff = -1;

    for (int i = startIndex; i < nNotes; i++) {
        MidiNote *currentNote = m_notes.Get(i);

        int keyDiff = abs((int)m_notes.Get(i)->m_midiKey - midiKey);
        int timeDiff = abs((int)m_notes.Get(i)->m_timeStamp - timeStamp);

        if (timeDiff < timeThreshold) {
            if (closestNote == nullptr) {
                closestNote = currentNote;
                refKeyDiff = keyDiff;
                refTimeDiff = timeDiff;
            }
            else {
                if (keyDiff < refKeyDiff) {
                    closestNote = currentNote;
                    refKeyDiff = keyDiff;
                    refTimeDiff = timeDiff;
                }
                else if (keyDiff == refKeyDiff) {
                    if (timeDiff < refTimeDiff) {
                        closestNote = currentNote;
                        refKeyDiff = keyDiff;
                        refTimeDiff = timeDiff;
                    }
                }
            }
        }
    }

    return closestNote;
}

int toccata::MidiTrack::GetStartDeltaTime() const {
    if (m_notes.GetNumObjects() <= 0) return 0;

    return m_notes.Get(0)->m_timeStamp;
}

int toccata::MidiTrack::GetEndDeltaTime() const {
    int nNotes = m_notes.GetNumObjects();

    if (nNotes <= 0) return 0;

    return m_notes.Get(nNotes - 1)->m_timeStamp;
}

toccata::MidiPianoSegment::MidiPianoSegment() {
    m_leftHand.m_parent = this;
    m_rightHand.m_parent = this;
}

toccata::MidiPianoSegment::~MidiPianoSegment() {
    /* void */
}

int toccata::MidiPianoSegment::ConvertDeltaTimeToNote(int deltaTime) const {
    if (deltaTime == 0) return 0;

    double noteLength = (m_ticksPerQuarterNote * 4.0) / deltaTime;

    return (int)(noteLength + 0.5);
}

int toccata::MidiPianoSegment::ConvertDeltaTimeToMilliseconds(int deltaTime, int tempoBPM) const {
    double tempo_us = 60.0 * 1000000 / (tempoBPM);

    double ms = deltaTime * (tempo_us / m_ticksPerQuarterNote) / 1000.0;

    return (int)(ms + 0.5);
}

int toccata::MidiPianoSegment::ConvertMillisecondsToDeltaTime(int ms, int tempoBPM) const {
    double tempo_us = 60.0 * 1000000 / (tempoBPM);
    double deltaTime = ((ms) / (tempo_us / m_ticksPerQuarterNote)) * 1000;

    int result = (int)(deltaTime + 0.5);

    return result;
}

int toccata::MidiPianoSegment::ConvertSecondsToDeltaTime(double s, int tempoBPM) const {
    double tempo_us = 60.0 * 1000000 / (tempoBPM);
    double deltaTime = s * m_ticksPerQuarterNote / tempo_us;

    return (int)(deltaTime + 0.5);
}

int toccata::MidiPianoSegment::GetStartDeltaTime() const {
    if (m_rightHand.GetNoteCount() == 0) return m_leftHand.GetStartDeltaTime();
    else if (m_leftHand.GetNoteCount() == 0) return m_rightHand.GetStartDeltaTime();

    return min(m_rightHand.GetStartDeltaTime(), m_leftHand.GetStartDeltaTime());
}

int toccata::MidiPianoSegment::GetEndDeltaTime() const {
    if (m_rightHand.GetNoteCount() == 0) return m_leftHand.GetEndDeltaTime();
    else if (m_leftHand.GetNoteCount() == 0) return m_rightHand.GetEndDeltaTime();

    return max(m_rightHand.GetEndDeltaTime(), m_leftHand.GetEndDeltaTime());
}

void toccata::MidiPianoSegment::SetTempoBPM(int bpm) {
    double t = 60.0 / bpm;
    t *= 1000000;

    m_tempo = (int)(t + 0.5);
}

int toccata::MidiPianoSegment::GetTempoBPM() const {
    double bpm = 60.0 / (m_tempo / 1000000.0);

    return (int)(bpm + 0.5);
}

void toccata::MidiPianoSegment::CopySettingsTo(MidiPianoSegment *targetSegment) const {
    targetSegment->m_negativeSMPTEFormat = m_negativeSMPTEFormat;
    targetSegment->m_tempo = m_tempo;
    targetSegment->m_ticksPerFrame = m_ticksPerFrame;
    targetSegment->m_ticksPerQuarterNote = m_ticksPerQuarterNote;
    targetSegment->m_timeFormat = m_timeFormat;
    targetSegment->m_timeSignatureDenominator = m_timeSignatureDenominator;
    targetSegment->m_timeSignatureNumerator = m_timeSignatureNumerator;
}

toccata::MidiFile::MidiFile() {
    m_generatedSegment = nullptr;
}

toccata::MidiFile::~MidiFile() {
    /* void */
}

void toccata::MidiFile::ByteSwap32(UINT32 *data) {
    UINT32 sdata = *data;

    *data = 
        ((sdata >> 24) & 0x000000ff) |
        ((sdata << 8) & 0x00ff0000) |
        ((sdata >> 8) & 0x0000ff00) |
        ((sdata << 24) & 0xff000000);
}

void toccata::MidiFile::ByteSwap16(UINT16 *data) {
    UINT32 sdata = *data;

    *data = 
        ((sdata >> 8) & 0x00ff) |
        ((sdata << 8) & 0xff00);
}

void toccata::MidiFile::ByteSwap24(UINT32 *data) {
    UINT32 sdata = *data;

    *data = 
        ((sdata >> 16) & 0x0000ff) |
        ((sdata) & 0x00ff00) |
        ((sdata << 16) & 0xff0000);
}

void toccata::MidiFile::Write(const char *fname, MidiPianoSegment *segment) {
    std::ofstream outputFile;
    outputFile.open(fname, std::ios::binary | std::ios::out);

    if (!outputFile.is_open()) {
        // Error opening file
        return;
    }

    int startDeltaTime = segment->GetStartDeltaTime();

    MidiTop topLevel;

    MidiChunk_Header *headerChunk = topLevel.NewChunk<MidiChunk_Header>();
    MidiChunk_Track *controlTrack = topLevel.NewChunk<MidiChunk_Track>();
    MidiChunk_Track *leftTrack = topLevel.NewChunk<MidiChunk_Track>();
    MidiChunk_Track *rightTrack = topLevel.NewChunk<MidiChunk_Track>();

    // Header chunk
    headerChunk->m_ticksPerQuarterNote = segment->m_ticksPerQuarterNote;
    headerChunk->m_trackCount = 3;

    // Control track
    MidiTrackEvent_Meta_TrackName *trackName = controlTrack->NewTrackEvent<MidiTrackEvent_Meta_TrackName>(startDeltaTime);
    trackName->SetTrackName("control: ");

    MidiTrackEvent_Meta_TimeSignature *timeSignature = controlTrack->NewTrackEvent<MidiTrackEvent_Meta_TimeSignature>(startDeltaTime);
    timeSignature->m_numerator = segment->GetTimeSignatureNumerator();
    timeSignature->m_denominator = segment->GetTimeSignatureDenominator();

    MidiTrackEvent_Meta_ChangeTempo *tempo = controlTrack->NewTrackEvent<MidiTrackEvent_Meta_ChangeTempo>(startDeltaTime);
    tempo->m_tempo = segment->GetTempo();

    MidiTrackEvent_Meta_EndOfTrack *endOfTrack = controlTrack->NewTrackEvent<MidiTrackEvent_Meta_EndOfTrack>(startDeltaTime);

    // Left and right hand	
    MidiTrackEvent_Meta_TrackName *leftTrackName = leftTrack->NewTrackEvent<MidiTrackEvent_Meta_TrackName>(startDeltaTime);
    leftTrackName->SetTrackName("Left-hand:");
    WriteTrackData(leftTrack, &segment->m_leftHand, segment, 0);

    MidiTrackEvent_Meta_TrackName *rightTrackName = rightTrack->NewTrackEvent<MidiTrackEvent_Meta_TrackName>(startDeltaTime);
    rightTrackName->SetTrackName("Right-hand:");
    WriteTrackData(rightTrack, &segment->m_rightHand, segment, 0);

    topLevel.WriteToFile(&outputFile);

    outputFile.close();
}

toccata::MidiPianoSegment *toccata::MidiFile::Read(const char *fname) {
    m_file.open(fname, std::ios::binary | std::ios::in);

    if (!m_file.is_open()) {
        // Error 

        // TEMP
        printf("Could not open file %s\n", fname);

        return nullptr;
    }

    m_generatedSegment = new MidiPianoSegment;

    while (true) {
        // Read chunks
        MidiChunkHeader chunkHeader;

        m_file.read((char *)&chunkHeader, sizeof(MidiChunkHeader));
        if (m_file.eof()) break;

        ByteSwap32(&chunkHeader.Length);

        m_chunkDataRemaining = chunkHeader.Length;

        ReadChunk(&chunkHeader);
    }

    // Reset all parameters
    MidiPianoSegment *segment = m_generatedSegment;

    // Close the file 
    m_file.close();

    // Clear variables
    m_generatedSegment = NULL;
    m_targetTrack = NULL;
    m_nTracks = 0;
    m_format = 0;

    return segment;
}

void toccata::MidiFile::WriteTrackData(MidiChunk_Track *target, MidiTrack *source, MidiPianoSegment *segment, int channel) {
    int nNotes = source->GetNoteCount();
    MidiNote **notes = source->GetBuffer();

    int currentTime = segment->GetStartDeltaTime();

    for (int i = 0; i < nNotes; i++) {
        MidiNote *note = notes[i];

        MidiTrackEvent_ChannelVoice_KeyDown *startEvent = target->NewTrackEvent<MidiTrackEvent_ChannelVoice_KeyDown>(note->m_timeStamp);
        startEvent->SetKey(note->m_midiKey);
        startEvent->SetVelocity(note->m_velocity);
        startEvent->SetChannel(channel);

        MidiTrackEvent_ChannelVoice_KeyDown *endEvent = target->NewTrackEvent<MidiTrackEvent_ChannelVoice_KeyDown>(note->m_timeStamp + note->m_noteLength);
        endEvent->SetKey(note->m_midiKey);
        endEvent->SetVelocity(0);
        endEvent->SetChannel(channel);

        currentTime = max(currentTime, note->m_timeStamp + note->m_noteLength);
    }

    MidiTrackEvent_Meta_EndOfTrack *endOfTrack = target->NewTrackEvent<MidiTrackEvent_Meta_EndOfTrack>(currentTime);
}

void toccata::MidiFile::ReadChunk(MidiChunkHeader *header) {
    if (header->Type[0] == 'M' &&
        header->Type[1] == 'T' &&
        header->Type[2] == 'h' &&
        header->Type[3] == 'd') 
    {
        MidiHeaderChunkData chunkData;

        // Read a header chunk
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
            m_generatedSegment->m_timeFormat = MidiPianoSegment::TimeFormat::Metrical;
            m_generatedSegment->m_ticksPerQuarterNote = chunkData.Division & 0x7FFF;

        }
        else {
            // Time-code time
            m_generatedSegment->m_timeFormat = MidiPianoSegment::TimeFormat::TimeCode;
            m_generatedSegment->m_negativeSMPTEFormat = (chunkData.Division & 0x7FFF) >> 8;
            m_generatedSegment->m_ticksPerFrame = (chunkData.Division & 0x00FF);
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
        m_targetTrack = &m_generatedSegment->m_rightHand;

        UINT32 currentTime = 0;

        // Read track events until there
        // are no bytes remaining
        while (m_chunkDataRemaining > 0) {
            currentTime += ReadTrackEvent(currentTime);
        }
    }
}

UINT32 toccata::MidiFile::ReadVariableLengthQuantity() {
    BYTE data;
    UINT32 VLQ = 0x00000000;

    int nBytes = 0;
    bool done = false;

    while (!done) {
        m_file.read((char *)&data, sizeof(BYTE));
        m_chunkDataRemaining -= sizeof(BYTE);

        if ((data & 0x80) == 0x00) {
            // Variable length quantity is done
            done = true;
        }

        VLQ <<= 7;
        VLQ |= data & 0x7F;

        nBytes++;
    }

    return VLQ;
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
                m_targetTrack = &m_generatedSegment->m_leftHand;
            }
            else {
                m_targetTrack = &m_generatedSegment->m_rightHand;
            }

            m_chunkDataRemaining -= length;
        }
        else if (metaType == 0x51) {
            // Change tempo

            UINT32 tempo = 0;
            m_file.read((char *)&tempo, sizeof(BYTE) * 3);

            ByteSwap24(&tempo);

            m_generatedSegment->m_tempo = tempo;
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

            numerator = ts[0];
            denominator = ts[1];

            m_generatedSegment->m_timeSignatureNumerator = (int)numerator;
            m_generatedSegment->m_timeSignatureDenominator = (int)denominator;

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
                MidiNote *newNote = m_targetTrack->AddNote();

                newNote->m_timeStamp = currentTime;
                newNote->m_midiKey = key;
                newNote->m_velocity = velocity;
            }
            else {
                // Note is released
                MidiNote *lastNote = m_targetTrack->FindLastNote(key, currentTime);

                lastNote->m_noteLength = currentTime - lastNote->m_timeStamp;
            }
        }
        else {
            /* void */
        }
    }

    return deltaTime;
}
