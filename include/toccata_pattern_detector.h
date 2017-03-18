#ifndef TOCCATA_PATTERN_DETECTOR_H
#define TOCCATA_PATTERN_DETECTOR_H

#include <yds_base.h>

#include "toccata_midi.h"

class Toccata_NoteMatch : public ysObject
{

public:

	Toccata_NoteMatch();
	~Toccata_NoteMatch();

	MidiNote *m_reference;
	MidiNote *m_played;

};

class Toccata_NoteInformation : public ysObject
{

public:

	enum HAND
	{

		HAND_UNKNOWN,
		HAND_LEFT,
		HAND_RIGHT,

	};

public:

	Toccata_NoteInformation();
	virtual ~Toccata_NoteInformation();

	HAND m_hand;
	Toccata_NoteMatch *m_noteMatch;
	MidiNote *m_playedNote;
	int m_timingError;

	bool m_correctNote;
	bool m_nonsenseNote;

};

class Toccata_Exercise;
class Toccata_PatternDetector : public ysObject
{

	friend class Toccata_PerformanceReport;
	friend class Toccata_Core;

	enum TRACK
	{

		TRACK_UNKNOWN,
		TRACK_LEFT_HAND,
		TRACK_RIGHT_HAND

	};

public:

	Toccata_PatternDetector();
	~Toccata_PatternDetector();

	void SetReferenceSegment(MidiPianoSegment *segment);
	void SetExercise(Toccata_Exercise *exercise);

	void ProcessNote(MidiNote *note);
	void Synchronize(MidiTrack *playedTrack, int currentDeltaTime);
	void CheckLock();
	void Reset();

	void SetCore(Toccata_Core *core) { m_core = core; }

	bool IsComplete() const { return m_complete; }
	bool IsLocked() const { return m_locked; }
	void WritePerformanceReport(Toccata_PerformanceReport *report, MidiPianoSegment *playedSegment);

	Toccata_Exercise *GetExercise() const { return m_exercise; }

protected:

	void ResolveNonsenseNoteHand();
	Toccata_NoteInformation *FindNearestHandHint(int index);
	void UpdateNonsenseNotes();

	int CalculateReferenceTimeStamp(MidiNote *note) const;
	int CalculatePlayedTimeStamp(MidiNote *note, MidiPianoSegment *playedSegment) const;

	void ProcessFirstNote(MidiNote *note);
	void ProcessTrack(int timeOffset, MidiTrack *playedTrack, TRACK referenceHand, bool strict=false);

	int GetMatchCount() const { return m_leftHandMatch.GetNumObjects() + m_rightHandMatch.GetNumObjects(); }

	MidiNote *FindClosestNote(MidiNote *note, int timeThreshold_ms, TRACK *track);

	ysDynamicArray<Toccata_NoteMatch, 4> m_leftHandMatch;
	ysDynamicArray<Toccata_NoteMatch, 4> m_rightHandMatch;
	ysDynamicArray<Toccata_NoteInformation, 4> m_noteInformation;

	Toccata_Exercise *m_exercise;
	MidiPianoSegment *m_segment;
	Toccata_Core *m_core;

	int m_processedNotes;
	int m_correctNotes;
	int m_errorCount;
	int m_missedNotesStreak;
	int m_nonsenseNotes;

	int m_playedNoteStartIndex;
	int m_currentDeltaTime;
	int m_startDeltaTime;
	int m_startReferenceDeltaTime;
	uint64_t m_startReferenceRealTime;

	bool m_complete;
	bool m_locked;

};

#endif