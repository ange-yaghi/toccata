#ifndef TOCCATA_RECORDER_H
#define TOCCATA_RECORDER_H

#include <mutex>

#include "toccata_midi.h"

class Toccata_InputBuffer
{

public:

	Toccata_InputBuffer();
	~Toccata_InputBuffer();

	MidiNote **GetNotes() { return m_noteBuffer.GetBuffer(); }

	void LockBuffer() { m_bufferLock.lock(); }
	void UnlockBuffer() { m_bufferLock.unlock(); }

	MidiNote *NewNote() { return m_noteBuffer.AddNote(); }

	MidiTrack *GetTrack() { return &m_noteBuffer; }

protected:

	MidiTrack m_noteBuffer;
	
	std::mutex m_bufferLock;

};

class Toccata_RawMidiFileOutput
{

public:

	Toccata_RawMidiFileOutput();
	~Toccata_RawMidiFileOutput();

	void Open(const char *fname);
	void WriteLine(DWORD wMsg, DWORD dwParam1, DWORD dwParam2);

protected:

	std::fstream m_file;

};

class Toccata_RawMidiFileInput
{

public:

	Toccata_RawMidiFileInput();
	~Toccata_RawMidiFileInput();

	void Open(const char *fname);
	bool ReadLine(DWORD *wMsg, DWORD *dwParam1, DWORD *dwParam2);

protected:

	std::fstream m_file;

};


class Toccata_Recorder
{

	friend class Toccata_Core;

public:

	enum FSM_STATE
	{

		STATE_RECORDING,
		STATE_PATTERN_CHECK,

	};

	enum ERROR_CODE
	{

		ERROR_NONE,
		ERROR_INVALID_INTERNAL_STATE,

	};

public:

	Toccata_Recorder();
	~Toccata_Recorder();

	void Initialize();

	ERROR_CODE OnNewTempo(int tempoBPM, int meterNumerator, int meterDenominator, bool metronomeEnabled);
	ERROR_CODE OnNewTime(int tempoBPM, int meterNumerator, int meterDenominator, bool metronomeEnabled);

	void ProcessMidiTick(int timeStamp);
	ERROR_CODE ProcessEvent(int midiKey, int velocity, int timeStamp, uint64_t systemTimeStamp);

	Toccata_InputBuffer *GetInputBuffer() { return &m_inputBuffer; }

	int OutstandingNoteCount();
	int ProcessInputBuffer(int *nOutstanding);

	int GetCurrentMidiTimeStamp() const { return m_currentMidiTimeStampExternal; }

	MidiPianoSegment *GetCurrentTarget() const { return m_currentTarget; }

	Toccata_RawMidiFileOutput *GetRawOutput() { return &m_rawOutput; }

	bool IsMetronomeEnabled() const { return m_isMetronomeEnabled; }
	int GetTempoBPM() { return m_tempoBPM; }
	int GetMeterNumerator() const { return m_meterNumerator; }
	int GetMeterDenominator() const { return m_meterDenominator; }

	bool IsInitialized() const { return m_initialized; }

protected:

	MidiPianoSegment *NewSegment();
	MidiPianoSegment *m_currentTarget;

	int m_midiTimeStampStart;
	int m_currentMidiTimeStamp;
	int m_currentMidiTimeStampExternal;
	bool m_recording;

protected:

	ysDynamicArray<MidiPianoSegment, 4> m_recordedSegments;

	Toccata_InputBuffer m_inputBuffer;

	Toccata_RawMidiFileOutput m_rawOutput;

protected:

	// States
	bool m_initialized;

	// Inputs
	bool m_isMetronomeEnabled;
	int m_tempoBPM;
	int m_meterNumerator;
	int m_meterDenominator;

};

#endif