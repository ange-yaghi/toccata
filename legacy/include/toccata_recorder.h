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

public:

	Toccata_Recorder();
	~Toccata_Recorder();

	void Initialize();

	void OnNewTempo();
	void OnNewTime();

	void ProcessMidiTick(int timeStamp);
	void ProcessNote(int midiKey, int velocity, int timeStamp, uint64_t systemTimeStamp);
	void IncrementTime(double dt);

	Toccata_InputBuffer *GetInputBuffer() { return &m_inputBuffer; }

	int OutstandingNoteCount();
	int ProcessInputBuffer(int *nOutstanding);

	int GetCurrentMidiTimeStamp() const { return m_currentMidiTimeStampExternal; }

	MidiPianoSegment *GetCurrentTarget() const { return m_currentTarget; }

	Toccata_RawMidiFileOutput *GetRawOutput() { return &m_rawOutput; }

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

	Toccata_Core *m_core;

};

#endif