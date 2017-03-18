#include "toccata_midi_output.h"
#include "toccata_midi.h"

/* MidiTop */
MidiTop::MidiTop()
{
}

MidiTop::~MidiTop()
{
}

void MidiTop::WriteToFile(std::ofstream *file)
{
	
	int nChunks = m_chunks.GetNumObjects();

	for (int i = 0; i < nChunks; i++)
	{

		m_chunks.Get(i)->WriteToFile(file);

	}

}

/* MidiChunk */
MidiChunk::MidiChunk(CHUNK_TYPE chunkType) : ysObject("MidiChunk")
{

	m_chunkType = chunkType;

}

MidiChunk::~MidiChunk()
{
}

void MidiChunk::WriteToFile(std::ofstream *file)
{

	MidiChunkHeader chunkHeader;
	
	if (m_chunkType == CHUNK_TYPE_HEADER)
	{

		chunkHeader.Type[0] = 'M';
		chunkHeader.Type[1] = 'T';
		chunkHeader.Type[2] = 'h';
		chunkHeader.Type[3] = 'd';

	}

	else if (m_chunkType == CHUNK_TYPE_TRACK)
	{

		chunkHeader.Type[0] = 'M';
		chunkHeader.Type[1] = 'T';
		chunkHeader.Type[2] = 'r';
		chunkHeader.Type[3] = 'k';

	}

	chunkHeader.Length = GetLength();
	MidiFile::ByteSwap32(&chunkHeader.Length);

	file->write((const char *)&chunkHeader, sizeof(MidiChunkHeader));

}

int MidiChunk::GetLength()
{

	return 0;

}

/* MidiChunk_Header */
MidiChunk_Header::MidiChunk_Header() : MidiChunk(CHUNK_TYPE_HEADER)
{

	m_ticksPerQuarterNote = 0;
	m_trackCount = 0;

}

MidiChunk_Header::~MidiChunk_Header()
{
}

int MidiChunk_Header::GetLength()
{

	return sizeof(MidiHeaderChunkData) + MidiChunk::GetLength();

}

void MidiChunk_Header::WriteToFile(std::ofstream *file)
{

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
MidiChunk_Track::MidiChunk_Track() : MidiChunk(CHUNK_TYPE_TRACK)
{
}

MidiChunk_Track::~MidiChunk_Track()
{
}

int MidiChunk_Track::GetLength()
{

	int trackEventSize = 0;

	int nTrackEvents = m_trackEvents.GetNumObjects();

	for (int i = 0; i < nTrackEvents; i++)
	{

		trackEventSize += m_trackEvents.Get(i)->GetSize();

	}

	return MidiChunk::GetLength() + trackEventSize;

}

int MidiChunk_Track::GetIndex(UINT32 deltaTime)
{

	// TEMP: basic slow implementation
	int trackEventSize = 0;

	int nTrackEvents = m_trackEvents.GetNumObjects();

	int left = 0;

	for (int i = 0; i < nTrackEvents - 1; i++)
	{

		if (deltaTime < m_trackEvents.Get(i)->m_absDeltaTime)
		{

			return left;

		}

		if (deltaTime >= m_trackEvents.Get(i)->m_absDeltaTime)
		{

			left = i + 1;

		}

	}

	return left;

}

void MidiChunk_Track::WriteToFile(std::ofstream *file)
{

	CalculateDeltaTimes();

	MidiChunk::WriteToFile(file);

	int nTrackEvents = m_trackEvents.GetNumObjects();

	for (int i = 0; i < nTrackEvents; i++)
	{

		m_trackEvents.Get(i)->WriteToFile(file);

	}

}

void MidiChunk_Track::CalculateDeltaTimes()
{

	UINT32 lastDeltaTime = 0;

	int nTrackEvents = m_trackEvents.GetNumObjects();

	if (nTrackEvents > 0)
	{

		lastDeltaTime = m_trackEvents.Get(0)->m_absDeltaTime;

	}

	for (int i = 0; i < nTrackEvents; i++)
	{

		m_trackEvents.Get(i)->m_deltaTime = m_trackEvents.Get(i)->m_absDeltaTime - lastDeltaTime;

		lastDeltaTime = m_trackEvents.Get(i)->m_absDeltaTime;

	}

	//if (nTrackEvents > 0)
	//{

	//	m_trackEvents.Get(nTrackEvents - 1)->m_deltaTime = 0;

	//}

}

/* MidiTrackEvent */
MidiTrackEvent::MidiTrackEvent(TRACK_EVENT_TYPE eventType) : ysObject("MidiTrackEvent")
{

	m_eventType = eventType;

	m_absDeltaTime = 0;
	m_deltaTime = 0;
	m_type = 0;

}

MidiTrackEvent::~MidiTrackEvent()
{
}

void MidiTrackEvent::WriteVariableLengthQuantity(std::ofstream *file, UINT32 vlq)
{

	bool done = false;

	int bytesNeeded = MidiTrackEvent::GetSizeForVLQ(vlq);

	for (int i = 0; i < bytesNeeded; i++)
	{

		int shift = 7 * (bytesNeeded - i - 1);
		BYTE data = (BYTE)((vlq & (0x7F << shift)) >> shift);

		if (i != bytesNeeded - 1) data |= 0x80;

		file->write((const char *)&data, sizeof(BYTE));

	}

}

int MidiTrackEvent::GetSizeForVLQ(UINT32 vlq)
{

	if (vlq < 128) return 1;
	if (vlq < 16384) return 2;
	if (vlq < 2097152) return 3;
	if (vlq < 268435456) return 4;
	return 0;

}


void MidiTrackEvent::WriteToFile(std::ofstream *file)
{

	WriteVariableLengthQuantity(file, m_deltaTime);
	file->write((const char *)&m_type, sizeof(BYTE));

}

int MidiTrackEvent::GetSize() const
{

	return sizeof(BYTE) + GetSizeForVLQ(m_deltaTime);

}

/* MidiTrackEvent_Meta */

MidiTrackEvent_Meta::MidiTrackEvent_Meta(META_EVENT_TYPE eventType) : MidiTrackEvent(EVENT_TYPE_META)
{

	m_metaEventType = eventType;
	m_length = 0;
	m_type = 0xFF;

}

MidiTrackEvent_Meta::~MidiTrackEvent_Meta()
{
}

void MidiTrackEvent_Meta::WriteToFile(std::ofstream *file)
{

	MidiTrackEvent::WriteToFile(file);

	file->write((const char *)&m_metaEventType, sizeof(BYTE));
	
	// Write the length
	WriteVariableLengthQuantity(file, m_length);

}

int MidiTrackEvent_Meta::GetSize() const
{

	return sizeof(BYTE) + GetSizeForVLQ(m_length) + m_length + MidiTrackEvent::GetSize();

}

/* MidiTrackEvent_Meta_Text */

MidiTrackEvent_Meta_Text::MidiTrackEvent_Meta_Text() : MidiTrackEvent_Meta(META_EVENT_TEXT)
{

	m_length = 0;
	m_text = NULL;

}

MidiTrackEvent_Meta_Text::~MidiTrackEvent_Meta_Text()
{

	if (m_text != NULL) delete[] m_text;

}

void MidiTrackEvent_Meta_Text::WriteToFile(std::ofstream *file)
{

	MidiTrackEvent_Meta::WriteToFile(file);

	if (m_text != NULL)
		file->write((const char *)m_text, m_length);

}

void MidiTrackEvent_Meta_Text::SetText(const char *text)
{

	if (m_text != NULL) delete[] text;
	m_text = NULL;
	m_length = 0;

	if (text != NULL)
	{

		int length = strlen(text);

		m_length = length;
		m_text = new char[m_length + 1];
		strcpy(m_text, text);

	}

}

/* MidiTrackEvent_Meta_TrackName */

MidiTrackEvent_Meta_TrackName::MidiTrackEvent_Meta_TrackName() : MidiTrackEvent_Meta(META_EVENT_TRACK_NAME)
{

	m_length = 0;
	m_trackName = NULL;

}

MidiTrackEvent_Meta_TrackName::~MidiTrackEvent_Meta_TrackName()
{

	if (m_trackName != NULL) delete[] m_trackName;

}

void MidiTrackEvent_Meta_TrackName::WriteToFile(std::ofstream *file)
{

	MidiTrackEvent_Meta::WriteToFile(file);

	if (m_trackName != NULL)
		file->write((const char *)m_trackName, m_length);

}

void MidiTrackEvent_Meta_TrackName::SetTrackName(const char *text)
{

	if (m_trackName != NULL) delete[] m_trackName;
	m_trackName = NULL;
	m_length = 0;

	if (text != NULL)
	{

		int length = strlen(text);

		m_length = length;
		m_trackName = new char[m_length + 1];
		strcpy(m_trackName, text);

	}

}

/* MidiTrackEvent_Meta_ChangeTempo */

MidiTrackEvent_Meta_ChangeTempo::MidiTrackEvent_Meta_ChangeTempo() : MidiTrackEvent_Meta(META_EVENT_CHANGE_TEMPO)
{

	m_tempo = 0;
	m_length = sizeof(BYTE) * 3;

}

MidiTrackEvent_Meta_ChangeTempo::~MidiTrackEvent_Meta_ChangeTempo()
{
}

void MidiTrackEvent_Meta_ChangeTempo::WriteToFile(std::ofstream *file)
{

	MidiTrackEvent_Meta::WriteToFile(file);

	UINT32 tempo = m_tempo;
	MidiFile::ByteSwap24(&tempo);

	file->write((const char *)&tempo, sizeof(BYTE) * 3);

}

/* MidiTrackEvent_Meta_EndOfTrack */

MidiTrackEvent_Meta_EndOfTrack::MidiTrackEvent_Meta_EndOfTrack() : MidiTrackEvent_Meta(META_EVENT_END_OF_TRACK)
{

	m_length = 0;

}

MidiTrackEvent_Meta_EndOfTrack::~MidiTrackEvent_Meta_EndOfTrack()
{
}

void MidiTrackEvent_Meta_EndOfTrack::WriteToFile(std::ofstream *file)
{

	MidiTrackEvent_Meta::WriteToFile(file);

}

/* MidiTrackEvent_Meta_TimeSignature */

MidiTrackEvent_Meta_TimeSignature::MidiTrackEvent_Meta_TimeSignature() : MidiTrackEvent_Meta(META_EVENT_TIME_SIGNATURE)
{

	m_numerator = 0;
	m_denominator = 0;

	m_length = sizeof(BYTE) * 4;

}

MidiTrackEvent_Meta_TimeSignature::~MidiTrackEvent_Meta_TimeSignature()
{
}

void MidiTrackEvent_Meta_TimeSignature::WriteToFile(std::ofstream *file)
{

	MidiTrackEvent_Meta::WriteToFile(file);

	UINT8 ts[4] = { m_numerator, m_denominator, 0x12, 0x08 };

	file->write((const char *)&ts, sizeof(BYTE)*4);

}

/* MidiTrackEvent_ChannelVoice */

MidiTrackEvent_ChannelVoice::MidiTrackEvent_ChannelVoice(CHANNEL_VOICE_EVENT_TYPE type) : MidiTrackEvent(EVENT_TYPE_META)
{

	m_channelVoiceEventType = type;

	m_channel = 0;
	m_status = 0;
	m_type = 0;

}

MidiTrackEvent_ChannelVoice::~MidiTrackEvent_ChannelVoice()
{
}

void MidiTrackEvent_ChannelVoice::WriteToFile(std::ofstream *file)
{

	MidiTrackEvent::WriteToFile(file);

	file->write((const char *)&m_byte1, sizeof(BYTE));
	file->write((const char *)&m_byte2, sizeof(BYTE));

}

void MidiTrackEvent_ChannelVoice::SetChannel(int channel)
{

	m_channel = channel;

	m_type &= 0xF0;
	m_type |= (channel & 0x0F);

}

void MidiTrackEvent_ChannelVoice::SetStatus(BYTE status)
{

	m_status = status;

	m_type &= 0x0F;
	m_type |= (status & 0xF0);

}

int MidiTrackEvent_ChannelVoice::GetSize() const
{

	return (2 * sizeof(BYTE)) + MidiTrackEvent::GetSize();

}

MidiTrackEvent_ChannelVoice_KeyDown::MidiTrackEvent_ChannelVoice_KeyDown() 
	: MidiTrackEvent_ChannelVoice(CHANNEL_VOICE_EVENT_KEY_DOWN)
{

	SetStatus(0x90);

}

MidiTrackEvent_ChannelVoice_KeyDown::~MidiTrackEvent_ChannelVoice_KeyDown()
{
}