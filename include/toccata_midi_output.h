#ifndef TOCCATA_MIDI_OUTPUT_H
#define TOCCATA_MIDI_OUTPUT_H

#include "toccata_midi.h"

#include <yds_base.h>

#include <Windows.h>
#include <iostream>
#include <fstream>

class MidiChunk : public ysObject
{

public:

	enum CHUNK_TYPE
	{

		CHUNK_TYPE_HEADER,
		CHUNK_TYPE_TRACK

	};

public:

	MidiChunk(CHUNK_TYPE chunkType);
	virtual ~MidiChunk();

	virtual void WriteToFile(std::ofstream *file);

	virtual int GetLength();

protected:

	CHUNK_TYPE m_chunkType;

};

class MidiTop
{

public:

	MidiTop();
	~MidiTop();

	template<typename ChunkType>
	ChunkType *NewChunk() { return m_chunks.NewGeneric<ChunkType>(); }

	void WriteToFile(std::ofstream *file);

protected:

	ysDynamicArray<MidiChunk, 4> m_chunks;

};

class MidiChunk_Header : public MidiChunk
{

public:

	MidiChunk_Header();
	virtual ~MidiChunk_Header();

	virtual void WriteToFile(std::ofstream *file);

	UINT16 m_ticksPerQuarterNote;
	UINT16 m_trackCount;

	virtual int GetLength();

};

class MidiTrackEvent : public ysObject
{

public:

	enum TRACK_EVENT_TYPE
	{

		EVENT_TYPE_SYSTEM_EXCLUSIVE,
		EVENT_TYPE_SYSTEM_EXCLUSIVE_RESUME,

		EVENT_TYPE_META,
		EVENT_TYPE_CHANNEL_VOICE,

	};

public:

	MidiTrackEvent(TRACK_EVENT_TYPE eventType);
	virtual ~MidiTrackEvent();

	static int GetSizeForVLQ(UINT32 vlq);
	static void WriteVariableLengthQuantity(std::ofstream *file, UINT32 vlq);

	virtual void WriteToFile(std::ofstream *file);
	virtual int GetSize() const;

	UINT32 m_deltaTime;
	UINT32 m_absDeltaTime;
	BYTE m_type;

	TRACK_EVENT_TYPE m_eventType;

};

class MidiChunk_Track : public MidiChunk
{

public:

	MidiChunk_Track();
	virtual ~MidiChunk_Track();

	virtual void WriteToFile(std::ofstream *file);

	virtual int GetLength();

	int GetIndex(UINT32 deltaTime);

	template<typename TrackEventType>
	TrackEventType *NewTrackEvent(int deltaTime) 
	{ 
		
		TrackEventType *newEvent = m_trackEvents.NewGeneric<TrackEventType>();
		int offset = GetIndex(deltaTime);

		m_trackEvents.Move(newEvent, offset);

		newEvent->m_absDeltaTime = deltaTime;

		return newEvent;

	}

protected:

	void CalculateDeltaTimes();

	ysDynamicArray<MidiTrackEvent, 4> m_trackEvents;

};

class MidiTrackEvent_Meta : public MidiTrackEvent
{

public:

	enum META_EVENT_TYPE
	{

		META_EVENT_TEXT = 0x01,
		META_EVENT_TRACK_NAME = 0x03,
		META_EVENT_CHANGE_TEMPO = 0x51,
		META_EVENT_END_OF_TRACK = 0x2F,
		META_EVENT_TIME_SIGNATURE = 0x58,

	};

public:

	MidiTrackEvent_Meta(META_EVENT_TYPE metaEventType);
	virtual ~MidiTrackEvent_Meta();

	virtual void WriteToFile(std::ofstream *file);
	virtual int GetSize() const;

	BYTE m_metaType;
	UINT32 m_length;

	META_EVENT_TYPE m_metaEventType;

};

class MidiTrackEvent_Meta_Text : public MidiTrackEvent_Meta
{

public:

	MidiTrackEvent_Meta_Text();
	~MidiTrackEvent_Meta_Text();

	virtual void WriteToFile(std::ofstream *file);

	void SetText(const char *text);

	char *m_text;

};

class MidiTrackEvent_Meta_TrackName : public MidiTrackEvent_Meta
{

public:

	MidiTrackEvent_Meta_TrackName();
	~MidiTrackEvent_Meta_TrackName();

	virtual void WriteToFile(std::ofstream *file);

	void SetTrackName(const char *text);

	char *m_trackName;

};

class MidiTrackEvent_Meta_ChangeTempo : public MidiTrackEvent_Meta
{

public:

	MidiTrackEvent_Meta_ChangeTempo();
	~MidiTrackEvent_Meta_ChangeTempo();

	virtual void WriteToFile(std::ofstream *file);

	UINT32 m_tempo;

};

class MidiTrackEvent_Meta_EndOfTrack : public MidiTrackEvent_Meta
{

public:

	MidiTrackEvent_Meta_EndOfTrack();
	~MidiTrackEvent_Meta_EndOfTrack();

	virtual void WriteToFile(std::ofstream *file);

};

class MidiTrackEvent_Meta_TimeSignature: public MidiTrackEvent_Meta
{

public:

	MidiTrackEvent_Meta_TimeSignature();
	~MidiTrackEvent_Meta_TimeSignature();

	virtual void WriteToFile(std::ofstream *file);

	UINT8 m_numerator;
	UINT8 m_denominator;

};

class MidiTrackEvent_ChannelVoice : public MidiTrackEvent
{

public:

	enum CHANNEL_VOICE_EVENT_TYPE
	{

		CHANNEL_VOICE_EVENT_KEY_DOWN,

	};

public:

	MidiTrackEvent_ChannelVoice(CHANNEL_VOICE_EVENT_TYPE eventType);
	~MidiTrackEvent_ChannelVoice();

	virtual void WriteToFile(std::ofstream *file);

	BYTE m_byte1;
	BYTE m_byte2;

	void SetChannel(int channel);
	void SetStatus(BYTE status);

	virtual int GetSize() const;

protected:

	CHANNEL_VOICE_EVENT_TYPE m_channelVoiceEventType;

	BYTE m_status;
	int m_channel;

};

class MidiTrackEvent_ChannelVoice_KeyDown : public MidiTrackEvent_ChannelVoice
{

public:

	MidiTrackEvent_ChannelVoice_KeyDown();
	~MidiTrackEvent_ChannelVoice_KeyDown();

	void SetVelocity(BYTE velocity) { m_byte2 = velocity; }
	void SetKey(BYTE key) { m_byte1 = key; }

};

#endif