#ifndef TOCCATA_CORE_H
#define TOCCATA_CORE_H

#include <yds_dynamic_array.h>
#include <yds_timing.h>

#include <atomic>
#include <stdint.h>

#include "toccata_fsm.h"
#include "toccata_sound_system.h"
#include "toccata_recorder.h"
#include "toccata_pattern_detection_system.h"
#include "toccata_session.h"
#include "toccata_database.h"
#include "toccata_error_reporting.h"

// Class declarations
class Toccata_SetTempoFSM;
class Toccata_SetTimeFSM;

class Toccata_Core
{

public:

	enum MODE
	{

		MODE_PIANO_SOURCE,
		MODE_FILE_SOURCE,

	};

public:

	static const char *Line;

public:

	Toccata_Core();
	~Toccata_Core();

	template <typename FSM_Type> FSM_Type *NewFSM()
	{

		FSM_Type *newFSM = m_fsms.NewGeneric<FSM_Type>(1);
		return newFSM;

	}

	void Initialize();
	void Update();
	void MainLoop();

	void ResetMetronome();
	void SetTempo(int tempoBPM);
	int GetTempoBPM() const;
	int GetTempo() const { return m_currentTempo; }

	void SetTime(int time);
	int GetTime() const { return m_currentTime; }
	uint64_t GetMetronomeOffset() const { return m_metronomeOffset; }

	void EnableMetronome(bool enable);
	bool IsMetronomeEnabled() const { return m_metronomeEnabled; }

	uint64_t GetTimeStamp() { return m_timingSystem.GetTime(); }

	const char *GetDirectory() { return m_session.GetFullDirectory().c_str(); }
	const char *GetTopDirectory() { return m_session.GetTopLevelDirectory().c_str(); }

	void AddExercise(const char *name, const char *midiReferenceFile);

	int NewPerformanceID();

	MODE GetMode() const { return m_mode; }

protected:

	void CreateFSMS();

public:

	// Publicly accessible FSM's
	Toccata_SetTempoFSM *SetTempoFSM() { return m_setTempoFSM; }
	Toccata_SetTimeFSM *SetTimeFSM() { return m_setTimeFSM; }

	// Subcomponents
	Toccata_Recorder	*GetRecorder() { return &m_recorder; }
	Toccata_Database	*GetDatabase() { return &m_database; }
	Toccata_Logger		*GetLogger() { return &m_logger; }

protected:

	ysDynamicArray<Toccata_FSM, 4> m_fsms;

	Toccata_SetTempoFSM *m_setTempoFSM;
	Toccata_SetTimeFSM *m_setTimeFSM;

	ysTimingSystem			m_timingSystem;
	Toccata_SoundSystem		m_soundSystem;
	Toccata_Recorder		m_recorder;
	Toccata_Session			m_session;
	Toccata_Logger			m_logger;

	Toccata_Database		m_database;

	// TEMP
	Toccata_PatternDetector	m_pattern;
	Toccata_PatternDetectionSystem m_patternDetector;

protected:

	// Current settings
	MODE m_mode;

	// TEMPORARY (put these in the metronome class)
	int m_currentTempo; // Tempo in usec/beat
	int m_currentTime;
	uint64_t m_currentTimer;
	uint64_t m_systemTime;

	bool m_metronomeEnabled;
	int m_metronomeTicks;
	uint64_t m_metronomeOffset;

};

extern Toccata_Core Toccata;

#endif