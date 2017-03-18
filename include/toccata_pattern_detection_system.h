#ifndef TOCCATA_PATTERN_DETECTION_SYSTEM_H
#define TOCCATA_PATTERN_DETECTION_SYSTEM_H

#include "toccata_pattern_detector.h"
#include "toccata_performance_report.h"

class Toccata_PatternDetectionSystem
{

	friend class Toccata_Core;

public:

	static const int DETECTORS_PER_SEGMENT = 2;

public:

	Toccata_PatternDetectionSystem();
	~Toccata_PatternDetectionSystem();

	void SetCore(Toccata_Core *core) { m_core = core; }
	void NewSegment(MidiPianoSegment *referenceSegment, Toccata_Exercise *associatedExercise);

	void ProcessNote(MidiNote *note);
	void Synchronize(MidiTrack *playedTrack, int currentDeltaTime);
	void Update();

	void ResetAllExceptLocked();

protected:

	ysDynamicArray<Toccata_PatternDetector, 4> m_patternDetectors;

	Toccata_Core *m_core;

	Toccata_PatternDetector *m_lockedDetector;

};

#endif