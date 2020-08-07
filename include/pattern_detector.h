#ifndef TOCCATA_CORE_PATTERN_DETECTOR_H
#define TOCCATA_CORE_PATTERN_DETECTOR_H

#include "midi.h"

#include "delta.h"

namespace toccata {

    class NoteMatch : public ysObject {
    public:
        NoteMatch();
        ~NoteMatch();

        MidiNote *m_reference;
        MidiNote *m_played;
    };

    class NoteInformation : public ysObject {
    public:
        enum class Hand {
            Unknown,
            Left,
            Right
        };

    public:
        NoteInformation();
        virtual ~NoteInformation();

        Hand m_hand;
        NoteMatch *m_noteMatch;
        MidiNote *m_playedNote;
        int m_timingError;

        bool m_correctNote;
        bool m_nonsenseNote;

    };

    class Exercise;
    class PerformanceReport;
    class Core;
    class PatternDetector : public ysObject {
        friend PerformanceReport;
        friend Core;

    public:
        PatternDetector();
        ~PatternDetector();

        void SetReferenceSegment(MidiPianoSegment *segment);
        void SetExercise(Exercise *exercise);

        void ProcessNote(MidiNote *note);
        void Synchronize(MidiTrack *playedTrack, int currentDeltaTime);
        void CheckLock();
        void Reset();

        void SetCore(Core *core) { m_core = core; }

        bool IsComplete() const { return m_complete; }
        bool IsLocked() const { return m_locked; }
        void WritePerformanceReport(PerformanceReport *report, MidiPianoSegment *playedSegment);

        Exercise *GetExercise() const { return m_exercise; }

    protected:

        void ResolveNonsenseNoteHand();
        NoteInformation *FindNearestHandHint(int index);
        void UpdateNonsenseNotes();

        int CalculateReferenceTimeStamp(MidiNote *note) const;
        int CalculatePlayedTimeStamp(MidiNote *note, MidiPianoSegment *playedSegment) const;

        void ProcessFirstNote(MidiNote *note);
        void ProcessTrack(int timeOffset, MidiTrack *playedTrack, NoteInformation::Hand referenceHand, bool strict = false);

        int GetMatchCount() const { return m_leftHandMatch.GetNumObjects() + m_rightHandMatch.GetNumObjects(); }

        MidiNote *FindClosestNote(MidiNote *note, int timeThreshold_ms, NoteInformation::Hand *track);

        ysDynamicArray<NoteMatch, 4> m_leftHandMatch;
        ysDynamicArray<NoteMatch, 4> m_rightHandMatch;
        ysDynamicArray<NoteInformation, 4> m_noteInformation;

        Exercise *m_exercise;
        MidiPianoSegment *m_segment;
        Core *m_core;

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

} /* namespace toccata */

#endif /* TOCCATA_CORE_PATTERN_DETECTOR_H */
