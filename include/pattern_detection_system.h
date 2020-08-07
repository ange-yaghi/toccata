#ifndef TOCCATA_CORE_PATTERN_DETECTION_SYSTEM_H
#define TOCCATA_CORE_PATTERN_DETECTION_SYSTEM_H

#include "pattern_detector.h"
#include "performance_report.h"

namespace toccata {

	class PatternDetectionSystem {
		friend class Core;

	public:
		static const int DetectorsPerSegment = 2;

	public:
		PatternDetectionSystem();
		~PatternDetectionSystem();

		void SetCore(Core *core) { m_core = core; }
		void NewSegment(MidiPianoSegment *referenceSegment, Exercise *associatedExercise);

		void ProcessNote(MidiNote *note);
		void Synchronize(MidiTrack *playedTrack, int currentDeltaTime);
		void Update();

		void ResetAllExceptLocked();

	protected:

		ysDynamicArray<PatternDetector, 4> m_patternDetectors;

		Core *m_core;

		PatternDetector *m_lockedDetector;
	};

} /* namespace toccata */

#endif /* TOCCATA_CORE_PATTERN_DETECTION_SYSTEM_H */
