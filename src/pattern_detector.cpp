#include "../include/pattern_detector.h"

#include "../include/exercise.h"
#include "../include/performance_report.h"
#include "../include/core.h"

#define CLOSER(target, a, b) (abs((target) - (a)) < abs((target) - (b)))
#define EQUIDISTANT(target, a, b) (abs((target) - (a)) == abs((target) - (b)))

toccata::NoteMatch::NoteMatch() : ysObject("NoteMatch") {
    m_reference = nullptr;
    m_played = nullptr;
}

toccata::NoteMatch::~NoteMatch() {
    /* void */
}

toccata::NoteInformation::NoteInformation() : ysObject("NoteInformation") {
    m_hand = Hand::Unknown;
    m_noteMatch = nullptr;
    m_correctNote = false;
    m_nonsenseNote = false;
    m_playedNote = nullptr;
    m_timingError = 0;
}

toccata::NoteInformation::~NoteInformation() {
    /* void */
}

toccata::PatternDetector::PatternDetector() : ysObject("PatternDetector") {
    m_errorCount = 0;
    m_startDeltaTime = 0;
    m_startReferenceDeltaTime = 0;
    m_processedNotes = 0;
    m_correctNotes = 0;
    m_nonsenseNotes = 0;
    m_playedNoteStartIndex = 0;
    m_currentDeltaTime = 0;
    m_complete = false;


    m_locked = false;

    m_segment = nullptr;
}

toccata::PatternDetector::~PatternDetector() {
    /* void */
}

void toccata::PatternDetector::SetReferenceSegment(MidiPianoSegment *segment) {
    m_segment = segment;
}

void toccata::PatternDetector::SetExercise(Exercise *exercise) {
    m_exercise = exercise;
}

void toccata::PatternDetector::ProcessNote(MidiNote *note) {
    if (m_processedNotes == 0) {
        m_playedNoteStartIndex = note->GetIndex();
    }

    // Make sure there are enough slots to hold note information
    int nNotes = note->GetParent()->GetNoteCount() - m_playedNoteStartIndex;
    int nRecords = m_noteInformation.GetNumObjects();

    m_processedNotes++;

    for (; nRecords < nNotes; nRecords++) {
        NoteInformation *info = m_noteInformation.New();
        info->m_playedNote = note->GetParent()->GetBuffer()[info->GetIndex() + m_playedNoteStartIndex];
    }

    // Process the note
    if (m_processedNotes == 1) {
        ProcessFirstNote(note);
    }
    else {
        // First look for exact matches
        ProcessTrack(note->m_timeStamp, note->GetParent(), NoteInformation::Hand::Right, true);
        ProcessTrack(note->m_timeStamp, note->GetParent(), NoteInformation::Hand::Left, true);

        ProcessTrack(note->m_timeStamp, note->GetParent(), NoteInformation::Hand::Right, false);
        ProcessTrack(note->m_timeStamp, note->GetParent(), NoteInformation::Hand::Left, false);

        // Reset the consecutive missed note counter
        m_missedNotesStreak = 0;
    }
}

void toccata::PatternDetector::Synchronize(MidiTrack *playedTrack, int currentDeltaTime) {
    if (m_processedNotes == 0) {
        // Can't really do much in this case because no notes are being played
    }
    else {
        // First look for exact matches
        ProcessTrack(currentDeltaTime, playedTrack, NoteInformation::Hand::Right, true);
        ProcessTrack(currentDeltaTime, playedTrack, NoteInformation::Hand::Left, true);

        ProcessTrack(currentDeltaTime, playedTrack, NoteInformation::Hand::Right, false);
        ProcessTrack(currentDeltaTime, playedTrack, NoteInformation::Hand::Left, false);

        int currentTime = playedTrack->GetSegment()->ConvertDeltaTimeToMilliseconds(m_currentDeltaTime - m_startDeltaTime, m_core->GetTempoBPM());
        int deltaTimeRef = m_segment->ConvertMillisecondsToDeltaTime(currentTime, m_core->GetTempoBPM());

        if (deltaTimeRef > m_segment->GetEndDeltaTime()) {
            printf("Song finished\n");

            m_complete = true;
        }
    }
}

toccata::MidiNote *toccata::PatternDetector::FindClosestNote(MidiNote *note, int timeThreshold_ms, NoteInformation::Hand *track) {
    int threshold = m_segment->ConvertMillisecondsToDeltaTime(timeThreshold_ms, m_core->GetTempoBPM());
    int timeStamp = CalculateReferenceTimeStamp(note);

    MidiNote *closestLeft = m_segment->m_leftHand.FindClosestNote(note->m_midiKey, timeStamp, threshold);
    MidiNote *closestRight = m_segment->m_rightHand.FindClosestNote(note->m_midiKey, timeStamp, threshold);

    if (closestLeft == nullptr) {
        if (closestRight == nullptr) {
            *track = NoteInformation::Hand::Unknown;
        }
        else {
            *track = NoteInformation::Hand::Right;
        }

        return closestRight;
    }

    if (closestRight == nullptr) {
        *track = NoteInformation::Hand::Left;
        return closestLeft;
    }

    if (CLOSER(note->m_midiKey, closestLeft->m_midiKey, closestRight->m_midiKey)) {
        *track = NoteInformation::Hand::Left;
        return closestLeft;
    }
    else if (EQUIDISTANT(note->m_midiKey, closestLeft->m_midiKey, closestRight->m_midiKey)) {
        if (CLOSER(timeStamp, closestLeft->m_timeStamp, closestRight->m_timeStamp)) {
            *track = NoteInformation::Hand::Left;
            return closestLeft;
        }
        else {
            *track = NoteInformation::Hand::Right;
            return closestRight;
        }
    }
    else {
        *track = NoteInformation::Hand::Right;
        return closestRight;
    }
}

void toccata::PatternDetector::ProcessTrack(int timeOffset, MidiTrack *playedTrack, NoteInformation::Hand referenceHand, bool strict) {
    MidiTrack *track = (referenceHand == NoteInformation::Hand::Left) ? &m_segment->m_leftHand : &m_segment->m_rightHand;
    ysDynamicArray<NoteMatch, 4> *noteMatches = (referenceHand == NoteInformation::Hand::Left) ? &m_leftHandMatch : &m_rightHandMatch;

    char tID = 'L';
    if (referenceHand == NoteInformation::Hand::Right) tID = 'R';

    int threshold = playedTrack->GetSegment()->ConvertMillisecondsToDeltaTime(m_core->GetTempo() / 2000, m_core->GetTempoBPM());

    MidiNote **referenceNotes = track->GetBuffer();
    int nNotes = track->GetNoteCount();

    if (timeOffset > m_currentDeltaTime) m_currentDeltaTime = timeOffset;

    for (int i = 0; i < nNotes; i++) {
        NoteMatch *noteMatch = nullptr;

        int noteTimeStamp = CalculatePlayedTimeStamp(referenceNotes[i], playedTrack->GetSegment());

        // Check that the note is safely in the past OR the strict flag is set
        if ((timeOffset - noteTimeStamp > threshold) || (strict && ((timeOffset - noteTimeStamp) >= 0))) {
            if (noteMatches->Get(i)->m_reference == nullptr) {
                MidiNote *closest = playedTrack->FindClosestNote(referenceNotes[i]->m_midiKey, noteTimeStamp, threshold, m_playedNoteStartIndex);
                bool validMatch = true;

                if (strict) {
                    if (closest == nullptr) validMatch = false;
                    else {
                        if (closest->m_midiKey != referenceNotes[i]->m_midiKey) {
                            validMatch = false;
                        }

                        NoteInformation *noteInfo = m_noteInformation.Get(closest->GetIndex() - m_playedNoteStartIndex);

                        if (noteInfo->m_noteMatch != nullptr) validMatch = false;
                    }
                }
                else {
                    // If the strict flag is not set the closest note will suffice
                }

                if (validMatch) {
                    if (closest != nullptr) {
                        int noteTimeStamp_ref = CalculateReferenceTimeStamp(closest);

                        NoteInformation::Hand track = NoteInformation::Hand::Unknown;
                        MidiNote *closestToPlayed = FindClosestNote(closest, 300, &track);

                        if (closestToPlayed != referenceNotes[i]) closest = nullptr;
                    }

                    noteMatch = noteMatches->Get(i);

                    noteMatch->m_played = closest;
                    noteMatch->m_reference = referenceNotes[i];

                    if (closest == nullptr) {
                        m_errorCount++;
                        m_missedNotesStreak++;
                        printf("%c Mistake %d: Missed Note | ", tID, m_errorCount);
                        noteMatch->m_reference->PrintDebug(m_core->GetTempoBPM());
                    }

                    else {
                        if (noteMatch->m_played->m_midiKey != noteMatch->m_reference->m_midiKey) {
                            m_errorCount++;
                            printf("%c Mistake %d: Wrong Note\n", tID, m_errorCount);

                            NoteInformation *noteInfo = m_noteInformation.Get(noteMatch->m_played->GetIndex() - m_playedNoteStartIndex);
                            noteInfo->m_hand = referenceHand;
                            noteInfo->m_noteMatch = noteMatch;
                            noteInfo->m_correctNote = false;
                        }
                        else {
                            int played_ms = noteMatch->m_played->GetTimeStampMilliseconds(-m_startDeltaTime, m_core->GetTempoBPM());
                            int ref_ms = noteMatch->m_reference->GetTimeStampMilliseconds(-m_startReferenceDeltaTime, m_core->GetTempoBPM());

                            printf("%c Correct note, ERR: %d |", tID, played_ms - ref_ms);
                            noteMatch->m_played->PrintDebug(m_core->GetTempoBPM());

                            NoteInformation *noteInfo = m_noteInformation.Get(noteMatch->m_played->GetIndex() - m_playedNoteStartIndex);
                            noteInfo->m_hand = referenceHand;
                            noteInfo->m_noteMatch = noteMatch;
                            noteInfo->m_correctNote = true;
                            noteInfo->m_timingError = played_ms - ref_ms;

                            m_correctNotes++;
                        }
                    }
                }
            }
        }
    }
}

void toccata::PatternDetector::ProcessFirstNote(MidiNote *note) {
    m_startDeltaTime = note->m_timeStamp;

    m_startReferenceDeltaTime = m_segment->m_rightHand.GetStartDeltaTime();
    ProcessTrack(note->m_timeStamp, note->GetParent(), NoteInformation::Hand::Right, true);

    int start_ms = m_segment->ConvertDeltaTimeToMilliseconds(m_startReferenceDeltaTime, m_core->GetTempoBPM());
    uint64_t approxStart = note->m_realTime - start_ms * 1000;
    uint64_t phase = (approxStart - m_core->GetMetronomeOffset()) % m_core->GetTempo();

    if (phase > m_core->GetTempo() / 2) {
        int inverse = m_core->GetTempo() - phase;
        m_startReferenceRealTime = approxStart + inverse;
    }
    else {
        m_startReferenceRealTime = approxStart - phase;
    }

    if (m_correctNotes == 0) {
        m_startReferenceDeltaTime = m_segment->m_leftHand.GetStartDeltaTime();
        ProcessTrack(note->m_timeStamp, note->GetParent(), NoteInformation::Hand::Left, true);
    }

    if (m_correctNotes == 0) {
        // Could not lock
        Reset();
    }
    else {
        // Set the start index for notes within the input track
    }
}

void toccata::PatternDetector::UpdateNonsenseNotes() {
    int nNotes = m_noteInformation.GetNumObjects();
    for (int i = 0; i < nNotes; i++) {
        NoteInformation *info = m_noteInformation.Get(i);

        if (info->m_noteMatch == nullptr && !info->m_nonsenseNote) {
            int threshold = info->m_playedNote->GetSegment()->ConvertMillisecondsToDeltaTime(m_core->GetTempo() / 2000, m_core->GetTempoBPM());
            if (m_currentDeltaTime - info->m_playedNote->m_timeStamp >= threshold) {
                info->m_nonsenseNote = true;
                m_nonsenseNotes++;
                m_errorCount++;

                printf("Mistake %d: Nonsense note ", m_errorCount);
                info->m_playedNote->PrintDebug(m_core->GetTempoBPM());
            }
        }
    }
}

void toccata::PatternDetector::CheckLock() {
    UpdateNonsenseNotes();

    if (m_processedNotes <= 10) {
        if (m_errorCount >= 5) {
            Reset();
        }
    }
    else {
        m_locked = true;

        float errorRate = m_errorCount / (float)m_processedNotes;
        if (errorRate > 0.5) {
            // There are somehow more mistakes than correct notes
            m_locked = false;
            Reset();
        }
    }

    if (m_missedNotesStreak >= 3) {
        m_locked = false;
        Reset();
    }
}

void toccata::PatternDetector::Reset() {
    m_processedNotes = 0;
    m_correctNotes = 0;
    m_missedNotesStreak = 0;
    m_errorCount = 0;
    m_nonsenseNotes = 0;

    m_currentDeltaTime = 0;
    m_startDeltaTime = 0;
    m_startReferenceDeltaTime = 0;
    m_startReferenceRealTime = 0;

    m_noteInformation.Clear();

    m_leftHandMatch.Clear();
    m_rightHandMatch.Clear();

    for (int i = 0; i < m_segment->m_leftHand.GetNoteCount(); i++) {
        m_leftHandMatch.New();
    }

    for (int i = 0; i < m_segment->m_rightHand.GetNoteCount(); i++) {
        m_rightHandMatch.New();
    }

    m_locked = false;
    m_complete = false;
}

int toccata::PatternDetector::CalculateReferenceTimeStamp(MidiNote *note) const {
    // Time Stamp Calculation
    int noteTimeStamp = note->m_timeStamp;

    // First convert the time stamp to milliseconds
    noteTimeStamp = note->GetTimeStampMilliseconds(-m_startDeltaTime, m_core->GetTempoBPM());

    // Now convert it to delta time within the current segment
    noteTimeStamp = m_segment->ConvertMillisecondsToDeltaTime(noteTimeStamp, m_core->GetTempoBPM()) + m_startReferenceDeltaTime;

    return noteTimeStamp;
}

int toccata::PatternDetector::CalculatePlayedTimeStamp(MidiNote *note, MidiPianoSegment *playedSegment) const {
    // Time Stamp Calculation
    int noteTimeStamp = note->m_timeStamp;

    // First convert the time stamp to milliseconds
    noteTimeStamp = note->GetTimeStampMilliseconds(-m_startReferenceDeltaTime, m_core->GetTempoBPM());

    // Now convert it to delta time within the played track
    noteTimeStamp = playedSegment->ConvertMillisecondsToDeltaTime(noteTimeStamp, m_core->GetTempoBPM()) + m_startDeltaTime;

    return noteTimeStamp;
}

void toccata::PatternDetector::WritePerformanceReport(PerformanceReport *report, MidiPianoSegment *playedSegment) {
    // Reset all report settings
    report->m_latencyCorrection = 0.0;

    report->ResetStats(&report->m_leftHandStats);
    report->ResetStats(&report->m_rightHandStats);

    report->SetMetronomeOffset(m_core->GetMetronomeOffset());

    // Copy segment settings (tempo, etc)
    playedSegment->CopySettingsTo(&report->m_playedSegment);
    m_segment->CopySettingsTo(&report->m_referenceSegment);

    // Make sure all notes have a hand
    ResolveNonsenseNoteHand();

    // Copy the played notes
    int nNotes = m_noteInformation.GetNumObjects();

    int timingErrorAccum = 0;
    int timingErrorSamples = 0;

    for (int i = 0; i < nNotes; i++) {

        NoteInformation *noteInformation = m_noteInformation.Get(i);
        PerformanceReport::PerformanceStats *stats;

        MidiAnalysisNote *newNote = nullptr;

        if (noteInformation->m_hand == NoteInformation::Hand::Left) {
            newNote = report->m_playedSegment.m_leftHand.AddAnalysisNote();
            stats = &report->m_leftHandStats;
        }
        else if (noteInformation->m_hand == NoteInformation::Hand::Right) {
            newNote = report->m_playedSegment.m_rightHand.AddAnalysisNote();
            stats = &report->m_rightHandStats;
        }
        else {
            // ERROR: Note does not have a hand (default to right hand)
            newNote = report->m_playedSegment.m_rightHand.AddAnalysisNote();
            stats = &report->m_rightHandStats;
        }

        newNote->Copy(noteInformation->m_playedNote);

        newNote->m_type = MidiAnalysisNote::NoteType::Played;
        newNote->m_timingError = (double)noteInformation->m_timingError;

        if (!noteInformation->m_correctNote) {
            if (!noteInformation->m_nonsenseNote) {
                newNote->m_errorType = MidiAnalysisNote::NoteErrorType::Wrong;
                stats->WrongNotes++;
            }
            else {
                newNote->m_errorType = MidiAnalysisNote::NoteErrorType::Nonsense;
                stats->NonsenseNotes++;
            }
        }
        else {
            MidiNote *referenceNote = noteInformation->m_noteMatch->m_reference;

            newNote->m_errorType = MidiAnalysisNote::NoteErrorType::None;

            newNote->m_realTimeError = (double)
                ((int)(newNote->m_realTime / 1000) - (int)(m_startReferenceRealTime / 1000) - referenceNote->GetTimeStampMilliseconds(0, m_core->GetTempoBPM()));

            stats->CorrectNotes++;

            timingErrorAccum += noteInformation->m_timingError;
            timingErrorSamples++;
        }
    }

    if (timingErrorSamples > 0) {
        report->m_latencyCorrection = (double)timingErrorAccum / timingErrorSamples;
    }
    else {
        report->m_latencyCorrection = 0.0;
    }

    report->m_latencyCorrected = false;

    // Copy the reference notes

    // TODO: put left hand and right hand logic in a function to reduce code duplication

    /* Left Hand */
    int nLeftHandNotes = m_leftHandMatch.GetNumObjects();

    for (int i = 0; i < nLeftHandNotes; i++) {
        MidiAnalysisNote *newNote = report->m_referenceSegment.m_leftHand.AddAnalysisNote();
        MidiNote *referenceNote = m_leftHandMatch.Get(i)->m_reference;
        MidiNote *playedNote = m_leftHandMatch.Get(i)->m_played;

        newNote->Copy(referenceNote);

        if (playedNote == nullptr) {
            newNote->m_errorType = MidiAnalysisNote::NoteErrorType::Missed;
            report->m_leftHandStats.MissedNotes++;
        }
        else {
            if (playedNote->m_midiKey != referenceNote->m_midiKey) {
                newNote->m_errorType = MidiAnalysisNote::NoteErrorType::Wrong;

                // Note: wrong notes are already counted by this point
            }
            else {
                newNote->m_errorType = MidiAnalysisNote::NoteErrorType::None;
            }
        }
    }

    /* Right Hand */
    int nRightHandNotes = m_rightHandMatch.GetNumObjects();

    for (int i = 0; i < nRightHandNotes; i++) {
        MidiAnalysisNote *newNote = report->m_referenceSegment.m_rightHand.AddAnalysisNote();
        MidiNote *referenceNote = m_rightHandMatch.Get(i)->m_reference;
        MidiNote *playedNote = m_rightHandMatch.Get(i)->m_played;

        newNote->Copy(referenceNote);

        if (playedNote == nullptr) {
            newNote->m_errorType = MidiAnalysisNote::NoteErrorType::Missed;
            report->m_rightHandStats.MissedNotes++;
        }
        else if (playedNote->m_midiKey != referenceNote->m_midiKey) {
            newNote->m_errorType = MidiAnalysisNote::NoteErrorType::Wrong;

            // Note: wrong notes are already counted by this point
        }
        else {
            newNote->m_errorType = MidiAnalysisNote::NoteErrorType::None;
        }
    }

    // Correct for latency in the system
    report->CorrectLatency();
}

void toccata::PatternDetector::ResolveNonsenseNoteHand() {
    // Assign a hand to the played notes that don't have hands
    int nNotes = m_noteInformation.GetNumObjects();

    for (int i = 0; i < nNotes; i++) {
        NoteInformation *noteInformation = m_noteInformation.Get(i);

        MidiNote *note = noteInformation->m_playedNote;
        NoteInformation::Hand hand = noteInformation->m_hand;

        if (hand == NoteInformation::Hand::Unknown) {
            NoteInformation *closest = FindNearestHandHint(i);
            noteInformation->m_hand = closest->m_hand;
        }
    }
}

toccata::NoteInformation *toccata::PatternDetector::FindNearestHandHint(int index) {
    // Find the closest note that has a known hand
    int nNotes = m_noteInformation.GetNumObjects();

    NoteInformation *closestNote = nullptr;
    int closestTimeDiff = 0;
    int closestKeyDiff = 0;

    MidiNote *ref = m_noteInformation.Get(index)->m_playedNote;

    for (int i = 0; i < nNotes; i++) {
        NoteInformation *noteInformation = m_noteInformation.Get(i);

        MidiNote *note = noteInformation->m_playedNote;
        NoteInformation::Hand hand = noteInformation->m_hand;

        int timeDiff = abs(note->m_timeStamp - ref->m_timeStamp);
        int keyDiff = abs(note->m_midiKey - ref->m_midiKey);

        if (hand != NoteInformation::Hand::Unknown) {
            if (closestNote == nullptr) {
                closestNote = noteInformation;
                closestTimeDiff = timeDiff;
                closestKeyDiff = keyDiff;
            }
            else {
                if (keyDiff < 5) {
                    if (timeDiff < closestTimeDiff) {
                        closestNote = noteInformation;
                        closestKeyDiff = keyDiff;
                        closestTimeDiff = timeDiff;
                    }
                }
            }
        }
    }

    return closestNote;
}
