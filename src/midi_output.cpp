#include "../include/midi_output.h"

#include "../include/midi.h"

/* MidiTop */
toccata::MidiTop::MidiTop() {
    /* void */
}

toccata::MidiTop::~MidiTop() {
    /* void */
}

void toccata::MidiTop::WriteToFile(std::ofstream *file) {
    int nChunks = m_chunks.GetNumObjects();

    for (int i = 0; i < nChunks; i++) {
        m_chunks.Get(i)->WriteToFile(file);
    }
}

/* MidiChunk */
toccata::MidiChunk::MidiChunk(ChunkType chunkType) : ysObject("MidiChunk") {
    m_chunkType = chunkType;
}

toccata::MidiChunk::~MidiChunk() {
    /* void */
}

void toccata::MidiChunk::WriteToFile(std::ofstream *file) {
    MidiChunkHeader chunkHeader;

    if (m_chunkType == ChunkType::Header) {
        chunkHeader.Type[0] = 'M';
        chunkHeader.Type[1] = 'T';
        chunkHeader.Type[2] = 'h';
        chunkHeader.Type[3] = 'd';
    }
    else if (m_chunkType == ChunkType::Track) {
        chunkHeader.Type[0] = 'M';
        chunkHeader.Type[1] = 'T';
        chunkHeader.Type[2] = 'r';
        chunkHeader.Type[3] = 'k';
    }

    chunkHeader.Length = GetLength();
    MidiFile::ByteSwap32(&chunkHeader.Length);

    file->write((const char *)&chunkHeader, sizeof(MidiChunkHeader));
}

int toccata::MidiChunk::GetLength() {
    return 0;
}

/* MidiChunk_Header */
toccata::MidiChunk_Header::MidiChunk_Header() : MidiChunk(ChunkType::Header) {
    m_ticksPerQuarterNote = 0;
    m_trackCount = 0;
}

toccata::MidiChunk_Header::~MidiChunk_Header() {
    /* void */
}

int toccata::MidiChunk_Header::GetLength() {
    return sizeof(MidiHeaderChunkData) + MidiChunk::GetLength();
}

void toccata::MidiChunk_Header::WriteToFile(std::ofstream *file) {
    MidiChunk::WriteToFile(file);

    MidiHeaderChunkData chunkData;

    // Change to 0x8000 if using the other division format
    chunkData.Division = 0x0000 | (m_ticksPerQuarterNote & 0x7FFF);
    chunkData.Format = FORMAT_1;
    chunkData.Ntrks = m_trackCount;

    MidiFile::ByteSwap16(&chunkData.Format);
    MidiFile::ByteSwap16(&chunkData.Division);
    MidiFile::ByteSwap16(&chunkData.Ntrks);

    file->write((const char *)&chunkData, sizeof(MidiHeaderChunkData));
}

/* MidiChunk_Track */
toccata::MidiChunk_Track::MidiChunk_Track() : MidiChunk(ChunkType::Track) {
    /* void */
}

toccata::MidiChunk_Track::~MidiChunk_Track() {
    /* void */
}

int toccata::MidiChunk_Track::GetLength() {
    int trackEventSize = 0;

    int nTrackEvents = m_trackEvents.GetNumObjects();

    for (int i = 0; i < nTrackEvents; i++) {
        trackEventSize += m_trackEvents.Get(i)->GetSize();
    }

    return MidiChunk::GetLength() + trackEventSize;
}

int toccata::MidiChunk_Track::GetIndex(UINT32 deltaTime) {
    // TEMP: basic slow implementation
    int trackEventSize = 0;
    int nTrackEvents = m_trackEvents.GetNumObjects();

    int left = 0;

    for (int i = 0; i < nTrackEvents - 1; i++) {
        if (deltaTime < m_trackEvents.Get(i)->m_absDeltaTime) {
            return left;
        }

        if (deltaTime >= m_trackEvents.Get(i)->m_absDeltaTime) {
            left = i + 1;
        }
    }

    return left;
}

void toccata::MidiChunk_Track::WriteToFile(std::ofstream *file) {
    CalculateDeltaTimes();

    MidiChunk::WriteToFile(file);

    int nTrackEvents = m_trackEvents.GetNumObjects();

    for (int i = 0; i < nTrackEvents; i++) {
        m_trackEvents.Get(i)->WriteToFile(file);
    }
}

void toccata::MidiChunk_Track::CalculateDeltaTimes() {
    UINT32 lastDeltaTime = 0;

    int nTrackEvents = m_trackEvents.GetNumObjects();

    if (nTrackEvents > 0) {
        lastDeltaTime = m_trackEvents.Get(0)->m_absDeltaTime;
    }

    for (int i = 0; i < nTrackEvents; i++) {
        m_trackEvents.Get(i)->m_deltaTime = m_trackEvents.Get(i)->m_absDeltaTime - lastDeltaTime;

        lastDeltaTime = m_trackEvents.Get(i)->m_absDeltaTime;
    }
}

/* MidiTrackEvent */
toccata::MidiTrackEvent::MidiTrackEvent(EventType eventType) : ysObject("MidiTrackEvent") {
    m_eventType = eventType;

    m_absDeltaTime = 0;
    m_deltaTime = 0;
    m_type = 0;
}

toccata::MidiTrackEvent::~MidiTrackEvent() {
    /* void */
}

void toccata::MidiTrackEvent::WriteVariableLengthQuantity(std::ofstream *file, UINT32 vlq) {
    bool done = false;

    int bytesNeeded = MidiTrackEvent::GetSizeForVLQ(vlq);

    for (int i = 0; i < bytesNeeded; i++) {
        int shift = 7 * (bytesNeeded - i - 1);
        BYTE data = (BYTE)((vlq & (0x7F << shift)) >> shift);

        if (i != bytesNeeded - 1) data |= 0x80;

        file->write((const char *)&data, sizeof(BYTE));
    }
}

int toccata::MidiTrackEvent::GetSizeForVLQ(UINT32 vlq) {
    if (vlq < 128) return 1;
    else if (vlq < 16384) return 2;
    else if (vlq < 2097152) return 3;
    else if (vlq < 268435456) return 4;
    else return 0;
}

void toccata::MidiTrackEvent::WriteToFile(std::ofstream *file) {
    WriteVariableLengthQuantity(file, m_deltaTime);
    file->write((const char *)&m_type, sizeof(BYTE));
}

int toccata::MidiTrackEvent::GetSize() const {
    return sizeof(BYTE) + GetSizeForVLQ(m_deltaTime);
}

/* MidiTrackEvent_Meta */

toccata::MidiTrackEvent_Meta::MidiTrackEvent_Meta(MetaEventType eventType) : MidiTrackEvent(EventType::Meta) {
    m_metaEventType = eventType;
    m_length = 0;
    m_type = 0xFF;
}

toccata::MidiTrackEvent_Meta::~MidiTrackEvent_Meta() {
    /* void */
}

void toccata::MidiTrackEvent_Meta::WriteToFile(std::ofstream *file) {
    MidiTrackEvent::WriteToFile(file);

    file->write((const char *)&m_metaEventType, sizeof(BYTE));

    // Write the length
    WriteVariableLengthQuantity(file, m_length);
}

int toccata::MidiTrackEvent_Meta::GetSize() const {
    return sizeof(BYTE) + GetSizeForVLQ(m_length) + m_length + MidiTrackEvent::GetSize();
}

/* MidiTrackEvent_Meta_Text */

toccata::MidiTrackEvent_Meta_Text::MidiTrackEvent_Meta_Text() : MidiTrackEvent_Meta(MetaEventType::Text) {
    m_length = 0;
    m_text = nullptr;
}

toccata::MidiTrackEvent_Meta_Text::~MidiTrackEvent_Meta_Text() {
    if (m_text != nullptr) delete[] m_text;
}

void toccata::MidiTrackEvent_Meta_Text::WriteToFile(std::ofstream *file) {
    MidiTrackEvent_Meta::WriteToFile(file);

    if (m_text != nullptr) {
        file->write((const char *)m_text, m_length);
    }
}

void toccata::MidiTrackEvent_Meta_Text::SetText(const char *text) {
    if (m_text != nullptr) delete[] text;
    m_text = nullptr;
    m_length = 0;

    if (text != nullptr) {
        int length = strlen(text);

        m_length = length;
        m_text = new char[m_length + 1];
        strcpy_s(m_text, m_length + 1, text);
    }
}

/* MidiTrackEvent_Meta_TrackName */

toccata::MidiTrackEvent_Meta_TrackName::MidiTrackEvent_Meta_TrackName() : MidiTrackEvent_Meta(MetaEventType::TrackName) {
    m_length = 0;
    m_trackName = nullptr;
}

toccata::MidiTrackEvent_Meta_TrackName::~MidiTrackEvent_Meta_TrackName() {
    if (m_trackName != nullptr) delete[] m_trackName;
}

void toccata::MidiTrackEvent_Meta_TrackName::WriteToFile(std::ofstream *file) {
    MidiTrackEvent_Meta::WriteToFile(file);

    if (m_trackName != nullptr) {
        file->write((const char *)m_trackName, m_length);
    }
}

void toccata::MidiTrackEvent_Meta_TrackName::SetTrackName(const char *text) {
    if (m_trackName != nullptr) delete[] m_trackName;
    m_trackName = nullptr;
    m_length = 0;

    if (text != nullptr) {
        int length = strlen(text);

        m_length = length;
        m_trackName = new char[m_length + 1];
        strcpy_s(m_trackName, m_length + 1, text);
    }
}

/* MidiTrackEvent_Meta_ChangeTempo */

toccata::MidiTrackEvent_Meta_ChangeTempo::MidiTrackEvent_Meta_ChangeTempo() : MidiTrackEvent_Meta(MetaEventType::ChangeTempo) {
    m_tempo = 0;
    m_length = sizeof(BYTE) * 3;
}

toccata::MidiTrackEvent_Meta_ChangeTempo::~MidiTrackEvent_Meta_ChangeTempo() {
    /* void */
}

void toccata::MidiTrackEvent_Meta_ChangeTempo::WriteToFile(std::ofstream *file) {
    MidiTrackEvent_Meta::WriteToFile(file);

    UINT32 tempo = m_tempo;
    MidiFile::ByteSwap24(&tempo);

    file->write((const char *)&tempo, sizeof(BYTE) * 3);
}

/* MidiTrackEvent_Meta_EndOfTrack */

toccata::MidiTrackEvent_Meta_EndOfTrack::MidiTrackEvent_Meta_EndOfTrack() : MidiTrackEvent_Meta(MetaEventType::EndOfTrack) {
    m_length = 0;
}

toccata::MidiTrackEvent_Meta_EndOfTrack::~MidiTrackEvent_Meta_EndOfTrack() {
    /* void */
}

void toccata::MidiTrackEvent_Meta_EndOfTrack::WriteToFile(std::ofstream *file) {
    MidiTrackEvent_Meta::WriteToFile(file);
}

/* MidiTrackEvent_Meta_TimeSignature */

toccata::MidiTrackEvent_Meta_TimeSignature::MidiTrackEvent_Meta_TimeSignature() : MidiTrackEvent_Meta(MetaEventType::TimeSignature) {
    m_numerator = 0;
    m_denominator = 0;

    m_length = sizeof(BYTE) * 4;
}

toccata::MidiTrackEvent_Meta_TimeSignature::~MidiTrackEvent_Meta_TimeSignature() {
    /* void */
}

void toccata::MidiTrackEvent_Meta_TimeSignature::WriteToFile(std::ofstream *file) {
    MidiTrackEvent_Meta::WriteToFile(file);

    UINT8 ts[4] = { m_numerator, m_denominator, 0x12, 0x08 };

    file->write((const char *)&ts, sizeof(BYTE) * 4);
}

/* MidiTrackEvent_ChannelVoice */

toccata::MidiTrackEvent_ChannelVoice::MidiTrackEvent_ChannelVoice(ChannelVoiceEventType type) : MidiTrackEvent(EventType::Meta) {
    m_channelVoiceEventType = type;

    m_channel = 0;
    m_status = 0;
    m_type = 0;
}

toccata::MidiTrackEvent_ChannelVoice::~MidiTrackEvent_ChannelVoice() {
    /* void */
}

void toccata::MidiTrackEvent_ChannelVoice::WriteToFile(std::ofstream *file) {
    MidiTrackEvent::WriteToFile(file);

    file->write((const char *)&m_byte1, sizeof(BYTE));
    file->write((const char *)&m_byte2, sizeof(BYTE));
}

void toccata::MidiTrackEvent_ChannelVoice::SetChannel(int channel) {
    m_channel = channel;

    m_type &= 0xF0;
    m_type |= (channel & 0x0F);
}

void toccata::MidiTrackEvent_ChannelVoice::SetStatus(BYTE status) {
    m_status = status;

    m_type &= 0x0F;
    m_type |= (status & 0xF0);
}

int toccata::MidiTrackEvent_ChannelVoice::GetSize() const {
    return (2 * sizeof(BYTE)) + MidiTrackEvent::GetSize();
}

toccata::MidiTrackEvent_ChannelVoice_KeyDown::MidiTrackEvent_ChannelVoice_KeyDown()
    : MidiTrackEvent_ChannelVoice(ChannelVoiceEventType::KeyDown) {

    SetStatus(0x90);
}

toccata::MidiTrackEvent_ChannelVoice_KeyDown::~MidiTrackEvent_ChannelVoice_KeyDown() {
    /* void */
}
