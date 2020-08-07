#ifndef TOCCATA_CORE_CORE_H
#define TOCCATA_CORE_CORE_H

#include "fsm.h"
#include "sound_system.h"
#include "recorder.h"
#include "pattern_detection_system.h"
#include "session.h"
#include "database.h"
#include "error_reporting.h"

#include "delta.h"

#include <atomic>
#include <stdint.h>

namespace toccata {

	// Class declarations
	class SetTempoFsm;
	class SetTimeFsm;

	class Core {
	private:
		static Core *s_mainCore;

	public:
		enum class Mode {
			PianoSource,
			FileSource,
		};

	public:
		static const char *Line;

	public:
		Core();
		~Core();

		static Core *Get();

		template <typename T_Fsm> T_Fsm *NewFsm() {
			T_Fsm *newFSM = m_fsms.NewGeneric<T_Fsm>();
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

		Mode GetMode() const { return m_mode; }

	protected:
		void CreateFsms();

	public:
		// Publicly accessible FSM's
		SetTempoFsm *SetTempoFsm() { return m_setTempoFSM; }
		SetTimeFsm *SetTimeFsm() { return m_setTimeFSM; }

		// Subcomponents
		Recorder *GetRecorder() { return &m_recorder; }
		Database *GetDatabase() { return &m_database; }
		Logger *GetLogger() { return &m_logger; }

	protected:
		ysDynamicArray<Fsm, 4> m_fsms;

		toccata::SetTempoFsm *m_setTempoFSM;
		toccata::SetTimeFsm *m_setTimeFSM;

		ysTimingSystem	m_timingSystem;
		SoundSystem		m_soundSystem;
		Recorder		m_recorder;
		Session			m_session;
		Logger			m_logger;

		Database		m_database;

		// TEMP
		PatternDetector	m_pattern;
		PatternDetectionSystem m_patternDetector;

	protected:
		// Current settings
		Mode m_mode;

		// TEMPORARY (put these in the metronome class)
		int m_currentTempo; // Tempo in usec/beat
		int m_currentTime;
		uint64_t m_currentTimer;
		uint64_t m_systemTime;

		bool m_metronomeEnabled;
		int m_metronomeTicks;
		uint64_t m_metronomeOffset;
	};

} /* namespace toccata */

#endif /* TOCCATA_CORE_CORE_H */
