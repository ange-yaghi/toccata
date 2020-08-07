#ifndef TOCCATA_CORE_MIDI_OUTPUT_H
#define TOCCATA_CORE_MIDI_OUTPUT_H

#include "midi.h"

#include "delta.h"

#include <Windows.h>
#include <iostream>
#include <fstream>

namespace toccata {

	class MidiChunk : public ysObject {
	public:
		enum class ChunkType {
			Header,
			Track
		};

	public:
		MidiChunk(ChunkType chunkType);
		virtual ~MidiChunk();

		virtual void WriteToFile(std::ofstream *file);

		virtual int GetLength();

	private:
		ChunkType m_chunkType;
	};

	class MidiTop {
	public:
		MidiTop();
		~MidiTop();

		template<typename ChunkType>
		ChunkType *NewChunk() { return m_chunks.NewGeneric<ChunkType>(); }

		void WriteToFile(std::ofstream *file);

	protected:
		ysDynamicArray<MidiChunk, 4> m_chunks;
	};

	class MidiChunk_Header : public MidiChunk {
	public:
		MidiChunk_Header();
		virtual ~MidiChunk_Header();

		virtual void WriteToFile(std::ofstream *file);

		UINT16 m_ticksPerQuarterNote;
		UINT16 m_trackCount;

		virtual int GetLength();
	};

	class MidiTrackEvent : public ysObject {
	public:
		enum EventType {
			SystemExclusive,
			SystemExclusiveResume,

			Meta,
			ChannelVoice,
		};

	public:

		MidiTrackEvent(EventType eventType);
		virtual ~MidiTrackEvent();

		static int GetSizeForVLQ(UINT32 vlq);
		static void WriteVariableLengthQuantity(std::ofstream *file, UINT32 vlq);

		virtual void WriteToFile(std::ofstream *file);
		virtual int GetSize() const;

		UINT32 m_deltaTime;
		UINT32 m_absDeltaTime;
		BYTE m_type;

		EventType m_eventType;
	};

	class MidiChunk_Track : public MidiChunk {
	public:
		MidiChunk_Track();
		virtual ~MidiChunk_Track();

		virtual void WriteToFile(std::ofstream *file);

		virtual int GetLength();

		int GetIndex(UINT32 deltaTime);

		template<typename TrackEventType>
		TrackEventType *NewTrackEvent(int deltaTime) {
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

	class MidiTrackEvent_Meta : public MidiTrackEvent {
	public:
		enum MetaEventType {
			Text = 0x01,
			TrackName = 0x03,
			ChangeTempo = 0x51,
			EndOfTrack = 0x2F,
			TimeSignature = 0x58,
		};

	public:
		MidiTrackEvent_Meta(MetaEventType metaEventType);
		virtual ~MidiTrackEvent_Meta();

		virtual void WriteToFile(std::ofstream *file);
		virtual int GetSize() const;

		BYTE m_metaType;
		UINT32 m_length;

		MetaEventType m_metaEventType;
	};

	class MidiTrackEvent_Meta_Text : public MidiTrackEvent_Meta {
	public:
		MidiTrackEvent_Meta_Text();
		~MidiTrackEvent_Meta_Text();

		virtual void WriteToFile(std::ofstream *file);

		void SetText(const char *text);

		char *m_text;
	};

	class MidiTrackEvent_Meta_TrackName : public MidiTrackEvent_Meta {
	public:
		MidiTrackEvent_Meta_TrackName();
		~MidiTrackEvent_Meta_TrackName();

		virtual void WriteToFile(std::ofstream *file);

		void SetTrackName(const char *text);

		char *m_trackName;
	};

	class MidiTrackEvent_Meta_ChangeTempo : public MidiTrackEvent_Meta {
	public:
		MidiTrackEvent_Meta_ChangeTempo();
		~MidiTrackEvent_Meta_ChangeTempo();

		virtual void WriteToFile(std::ofstream *file);

		UINT32 m_tempo;
	};

	class MidiTrackEvent_Meta_EndOfTrack : public MidiTrackEvent_Meta {
	public:
		MidiTrackEvent_Meta_EndOfTrack();
		~MidiTrackEvent_Meta_EndOfTrack();

		virtual void WriteToFile(std::ofstream *file);
	};

	class MidiTrackEvent_Meta_TimeSignature : public MidiTrackEvent_Meta {
	public:
		MidiTrackEvent_Meta_TimeSignature();
		~MidiTrackEvent_Meta_TimeSignature();

		virtual void WriteToFile(std::ofstream *file);

		UINT8 m_numerator;
		UINT8 m_denominator;
	};

	class MidiTrackEvent_ChannelVoice : public MidiTrackEvent {
	public:
		enum ChannelVoiceEventType {
			KeyDown,
		};

	public:
		MidiTrackEvent_ChannelVoice(ChannelVoiceEventType eventType);
		~MidiTrackEvent_ChannelVoice();

		virtual void WriteToFile(std::ofstream *file);

		BYTE m_byte1;
		BYTE m_byte2;

		void SetChannel(int channel);
		void SetStatus(BYTE status);

		virtual int GetSize() const;

	protected:
		ChannelVoiceEventType m_channelVoiceEventType;

		BYTE m_status;
		int m_channel;
	};

	class MidiTrackEvent_ChannelVoice_KeyDown : public MidiTrackEvent_ChannelVoice {
	public:
		MidiTrackEvent_ChannelVoice_KeyDown();
		~MidiTrackEvent_ChannelVoice_KeyDown();

		void SetVelocity(BYTE velocity) { m_byte2 = velocity; }
		void SetKey(BYTE key) { m_byte1 = key; }
	};

} /* namespace toccata */

#endif /* TOCCATA_CORE_MIDI_OUTPUT_H */
