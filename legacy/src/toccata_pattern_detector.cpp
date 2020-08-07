#include "toccata_pattern_detector.h"
#include "toccata_exercise.h"
#include "toccata_performance_report.h"
#include "toccata_core.h"

#define CLOSER(target, a, b) (abs((target) - (a)) < abs((target) - (b)))
#define EQUIDISTANT(target, a, b) (abs((target) - (a)) == abs((target) - (b)))

Toccata_NoteMatch::Toccata_NoteMatch() : ysObject("Toccata_NoteMatch")
{

	m_reference = NULL;
	m_played = NULL;

}

Toccata_NoteMatch::~Toccata_NoteMatch()
{
}

Toccata_NoteInformation::Toccata_NoteInformation() : ysObject("Toccata_NoteInformation")
{

	m_hand = HAND_UNKNOWN;
	m_noteMatch = NULL;
	m_correctNote = false;
	m_nonsenseNote = false;
	m_playedNote = NULL;

}

Toccata_NoteInformation::~Toccata_NoteInformation()
{
}

Toccata_PatternDetector::Toccata_PatternDetector() : ysObject("Toccata_PatternDetector")
{

	m_errorCount = 0;
	m_startDeltaTime = 0;
	m_startReferenceDeltaTime = 0;
	m_processedNotes = 0;
	m_correctNotes = 0;
	m_nonsenseNotes = 0;
	m_playedNoteStartIndex = 0;
	m_currentDeltaTime = 0;

	m_locked = false;

	m_segment = NULL;

}

Toccata_PatternDetector::~Toccata_PatternDetector()
{
}

void Toccata_PatternDetector::SetReferenceSegment(MidiPianoSegment *segment)
{

	m_segment = segment;

}

void Toccata_PatternDetector::SetExercise(Toccata_Exercise *exercise)
{

	m_exercise = exercise;

}

void Toccata_PatternDetector::ProcessNote(MidiNote *note)
{

	if (m_processedNotes == 0)
	{
		
		m_playedNoteStartIndex = note->GetIndex();
		//printf("First note: %d\n", m_playedNoteStartIndex);

	}

	//printf("Processing: "); note->PrintDebug(m_core->GetTempo());

	// Make sure there are enough slots to hold note information
	int nNotes = note->GetParent()->GetNoteCount() - m_playedNoteStartIndex;
	int nRecords = m_noteInformation.GetNumObjects();

	m_processedNotes++;

	for (; nRecords < nNotes; nRecords++)
	{

		Toccata_NoteInformation *info = m_noteInformation.New();
		info->m_playedNote = note->GetParent()->GetBuffer()[info->GetIndex() + m_playedNoteStartIndex];

	}

	// Process the note
	if (m_processedNotes == 1)
	{

		ProcessFirstNote(note);

	}

	else
	{

		// First look for exact matches
		ProcessTrack(note->m_timeStamp, note->GetParent(), TRACK_RIGHT_HAND, true);
		ProcessTrack(note->m_timeStamp, note->GetParent(), TRACK_LEFT_HAND, true);

		ProcessTrack(note->m_timeStamp, note->GetParent(), TRACK_RIGHT_HAND, false);
		ProcessTrack(note->m_timeStamp, note->GetParent(), TRACK_LEFT_HAND, false);

		// Reset the consecutive missed note counter
		m_missedNotesStreak = 0;

	}

}

void Toccata_PatternDetector::Synchronize(MidiTrack *playedTrack, int currentDeltaTime)
{

	if (m_processedNotes == 0)
	{

		// Can't really do much in this case because no notes are being played

	}

	else
	{

		// First look for exact matches
		ProcessTrack(currentDeltaTime, playedTrack, TRACK_RIGHT_HAND, true);
		ProcessTrack(currentDeltaTime, playedTrack, TRACK_LEFT_HAND, true);

		ProcessTrack(currentDeltaTime, playedTrack, TRACK_RIGHT_HAND, false);
		ProcessTrack(currentDeltaTime, playedTrack, TRACK_LEFT_HAND, false);

		int currentTime = playedTrack->GetSegment()->ConvertDeltaTimeToMilliseconds(m_currentDeltaTime - m_startDeltaTime, m_core->GetTempoBPM());
		int deltaTimeRef = m_segment->ConvertMillisecondsToDeltaTime(currentTime, m_core->GetTempoBPM());

		if (deltaTimeRef > m_segment->GetEndDeltaTime())
		{

			printf("Song finished\n");

			m_complete = true;

			// TEMP
			//Reset();

		}

	}

}

MidiNote *Toccata_PatternDetector::FindClosestNote(MidiNote *note, int timeThreshold_ms, TRACK *track)
{

	int threshold = m_segment->ConvertMillisecondsToDeltaTime(timeThreshold_ms, m_core->GetTempoBPM());
	int timeStamp = CalculateReferenceTimeStamp(note);

	MidiNote *closestLeft = m_segment->m_leftHand.FindClosestNote(note->m_midiKey, timeStamp, threshold);
	MidiNote *closestRight = m_segment->m_rightHand.FindClosestNote(note->m_midiKey, timeStamp, threshold);

	if (closestLeft == NULL)
	{

		if (closestRight == NULL)
			*track = TRACK_UNKNOWN;

		else
			*track = TRACK_RIGHT_HAND;

		return closestRight;

	}

	if (closestRight == NULL)
	{

		*track = TRACK_LEFT_HAND;
		return closestLeft;

	}

	if (CLOSER(note->m_midiKey, closestLeft->m_midiKey, closestRight->m_midiKey))
	{

		*track = TRACK_LEFT_HAND;
		return closestLeft;

	}

	else if (EQUIDISTANT(note->m_midiKey, closestLeft->m_midiKey, closestRight->m_midiKey))
	{

		if (CLOSER(timeStamp, closestLeft->m_timeStamp, closestRight->m_timeStamp))
		{

			*track = TRACK_LEFT_HAND;
			return closestLeft;

		}

		else
		{

			*track = TRACK_RIGHT_HAND;
			return closestRight;

		}
	
	}

	else
	{

		*track = TRACK_RIGHT_HAND;
		return closestRight;

	}

}

void Toccata_PatternDetector::ProcessTrack(int timeOffset, MidiTrack *playedTrack, TRACK referenceHand, bool strict)
{

	MidiTrack *track = (referenceHand == TRACK_LEFT_HAND) ? &m_segment->m_leftHand : &m_segment->m_rightHand;
	ysDynamicArray<Toccata_NoteMatch, 4> *noteMatches = (referenceHand == TRACK_LEFT_HAND) ? &m_leftHandMatch : &m_rightHandMatch;

	char tID = 'L';
	if (referenceHand == TRACK_RIGHT_HAND) tID = 'R';

	int threshold = playedTrack->GetSegment()->ConvertMillisecondsToDeltaTime(m_core->GetTempo() / 2000, m_core->GetTempoBPM());

	MidiNote **referenceNotes = track->GetBuffer();
	int nNotes = track->GetNoteCount();

	if (timeOffset > m_currentDeltaTime) m_currentDeltaTime = timeOffset;

	for (int i = 0; i < nNotes; i++)
	{

		Toccata_NoteMatch *noteMatch = NULL;

		int noteTimeStamp = CalculatePlayedTimeStamp(referenceNotes[i], playedTrack->GetSegment());

		// Check that the note is safely in the past OR the strict flag is set
		if ((timeOffset - noteTimeStamp > threshold) || (strict && ((timeOffset - noteTimeStamp) >= 0)))
		{

			//if (referenceNotes[i]->GetIndex() == noteMatches->GetNumObjects())
			//{

			if (noteMatches->Get(i)->m_reference == NULL)
			{

				MidiNote *closest = playedTrack->FindClosestNote(referenceNotes[i]->m_midiKey, noteTimeStamp, threshold, m_playedNoteStartIndex);
				bool validMatch = true;

				if (strict)
				{

					if (closest == NULL) validMatch = false;
					else
					{

						if (closest->m_midiKey != referenceNotes[i]->m_midiKey)
						{

							validMatch = false;

						}

						Toccata_NoteInformation *noteInfo = m_noteInformation.Get(closest->GetIndex() - m_playedNoteStartIndex);

						if (noteInfo->m_noteMatch != NULL) validMatch = false;

					}

				}

				else
				{

					// If the strict flag is not set the closest note will suffice

				}

				if (validMatch)
				{

					if (closest != NULL)
					{

						int noteTimeStamp_ref = CalculateReferenceTimeStamp(closest);

						TRACK track = TRACK_UNKNOWN;
						MidiNote *closestToPlayed = FindClosestNote(closest, 300, &track);

						if (closestToPlayed != referenceNotes[i]) closest = NULL;

					}

					noteMatch = noteMatches->Get(i);

					noteMatch->m_played = closest;
					noteMatch->m_reference = referenceNotes[i];

					if (closest == NULL)
					{

						m_errorCount++;
						m_missedNotesStreak++;
						printf("%c Mistake %d: Missed Note | ", tID, m_errorCount);
						noteMatch->m_reference->PrintDebug(m_core->GetTempoBPM());

					}

					else
					{

						if (noteMatch->m_played->m_midiKey != noteMatch->m_reference->m_midiKey)
						{

							m_errorCount++;
							printf("%c Mistake %d: Wrong Note\n", tID, m_errorCount);

							Toccata_NoteInformation *noteInfo = m_noteInformation.Get(noteMatch->m_played->GetIndex() - m_playedNoteStartIndex);
							noteInfo->m_hand = (Toccata_NoteInformation::HAND)referenceHand;
							noteInfo->m_noteMatch = noteMatch;
							noteInfo->m_correctNote = false;

						}

						else
						{

							int played_ms = noteMatch->m_played->GetTimeStampMilliseconds(-m_startDeltaTime, m_core->GetTempoBPM());
							int ref_ms = noteMatch->m_reference->GetTimeStampMilliseconds(-m_startReferenceDeltaTime, m_core->GetTempoBPM());

							printf("%c Correct note, ERR: %d |", tID, played_ms - ref_ms);
							noteMatch->m_played->PrintDebug(m_core->GetTempoBPM());

							Toccata_NoteInformation *noteInfo = m_noteInformation.Get(noteMatch->m_played->GetIndex() - m_playedNoteStartIndex);
							noteInfo->m_hand = (Toccata_NoteInformation::HAND)referenceHand;
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

void Toccata_PatternDetector::ProcessFirstNote(MidiNote *note)
{

	m_startDeltaTime = note->m_timeStamp;

	m_startReferenceDeltaTime = m_segment->m_rightHand.GetStartDeltaTime();
	ProcessTrack(note->m_timeStamp, note->GetParent(), TRACK_RIGHT_HAND, true);

	int start_ms = m_segment->ConvertDeltaTimeToMilliseconds(m_startReferenceDeltaTime, m_core->GetTempoBPM());
	uint64_t approxStart = note->m_realTime - start_ms * 1000;
	uint64_t phase = (approxStart - m_core->GetMetronomeOffset()) % m_core->GetTempo();

	if (phase > m_core->GetTempo() / 2)
	{

		int inverse = m_core->GetTempo() - phase;

		m_startReferenceRealTime = approxStart + inverse;

	}

	else
	{

		m_startReferenceRealTime = approxStart - phase;

	}

	if (m_correctNotes == 0)
	{

		m_startReferenceDeltaTime = m_segment->m_leftHand.GetStartDeltaTime();
		ProcessTrack(note->m_timeStamp, note->GetParent(), TRACK_LEFT_HAND, true);

	}

	if (m_correctNotes == 0)
	{

		// Could not lock
		//("No lock\n");
		Reset();

	}

	else
	{

		// Set the start index for notes within the input track
		//printf("Locked\n");

	}

}

void Toccata_PatternDetector::UpdateNonsenseNotes()
{

	int nNotes = m_noteInformation.GetNumObjects();

	for (int i = 0; i < nNotes; i++)
	{

		Toccata_NoteInformation *info = m_noteInformation.Get(i);

		if (info->m_noteMatch == NULL && !info->m_nonsenseNote)
		{

			int threshold = info->m_playedNote->GetSegment()->ConvertMillisecondsToDeltaTime(m_core->GetTempo() / 2000, m_core->GetTempoBPM());
			if (m_currentDeltaTime - info->m_playedNote->m_timeStamp >= threshold)
			{

				info->m_nonsenseNote = true;
				m_nonsenseNotes++;
				m_errorCount++;

				printf("Mistake %d: Nonsense note ", m_errorCount);
				info->m_playedNote->PrintDebug(m_core->GetTempoBPM());

			}

		}

	}

}

void Toccata_PatternDetector::CheckLock()
{

	UpdateNonsenseNotes();

	if (m_processedNotes <= 10)
	{

		if (m_errorCount >= 5)
		{

			Reset();

		}

	}

	else
	{

		m_locked = true;

		float errorRate = m_errorCount / (float)m_processedNotes;

		if (errorRate > 0.5)
		{

			// There are somehow more mistakes than correct notes
			m_locked = false;
			Reset();

		}

	}

	if (m_missedNotesStreak >= 3)
	{

		m_locked = false;
		Reset();

	}

}

void Toccata_PatternDetector::Reset()
{

	//printf("Reset (unlocked)...\n");

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

	for (int i = 0; i < m_segment->m_leftHand.GetNoteCount(); i++)
	{

		m_leftHandMatch.New();

	}

	for (int i = 0; i < m_segment->m_rightHand.GetNoteCount(); i++)
	{

		m_rightHandMatch.New();

	}

	m_locked = false;
	m_complete = false;

}

int Toccata_PatternDetector::CalculateReferenceTimeStamp(MidiNote *note) const
{

	/* Time Stamp Calculation */
	int noteTimeStamp = note->m_timeStamp;

	// First convert the time stamp to milliseconds
	noteTimeStamp = note->GetTimeStampMilliseconds(-m_startDeltaTime, m_core->GetTempoBPM());

	// Now convert it to delta time within the current segment
	noteTimeStamp = m_segment->ConvertMillisecondsToDeltaTime(noteTimeStamp, m_core->GetTempoBPM()) + m_startReferenceDeltaTime;

	return noteTimeStamp;

}

int Toccata_PatternDetector::CalculatePlayedTimeStamp(MidiNote *note, MidiPianoSegment *playedSegment) const
{

	/* Time Stamp Calculation */
	int noteTimeStamp = note->m_timeStamp;

	// First convert the time stamp to milliseconds
	noteTimeStamp = note->GetTimeStampMilliseconds(-m_startReferenceDeltaTime, m_core->GetTempoBPM());

	// Now convert it to delta time within the played track
	noteTimeStamp = playedSegment->ConvertMillisecondsToDeltaTime(noteTimeStamp, m_core->GetTempoBPM()) + m_startDeltaTime;

	return noteTimeStamp;

}

void Toccata_PatternDetector::WritePerformanceReport(Toccata_PerformanceReport *report, MidiPianoSegment *playedSegment)
{

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

	for (int i = 0; i < nNotes; i++)
	{

		Toccata_NoteInformation *noteInformation = m_noteInformation.Get(i);
		Toccata_PerformanceReport::PerformanceStats *stats;

		MidiAnalysisNote *newNote = NULL;

		if (noteInformation->m_hand == Toccata_NoteInformation::HAND_LEFT)
		{

			newNote = report->m_playedSegment.m_leftHand.AddAnalysisNote();
			stats = &report->m_leftHandStats;

		}

		else if (noteInformation->m_hand == Toccata_NoteInformation::HAND_RIGHT)
		{

			newNote = report->m_playedSegment.m_rightHand.AddAnalysisNote();
			stats = &report->m_rightHandStats;

		}

		else
		{

			// ERROR: Note does not have a hand (default to right hand)
			newNote = report->m_playedSegment.m_rightHand.AddAnalysisNote();
			stats = &report->m_rightHandStats;

		}

		newNote->Copy(noteInformation->m_playedNote);
		
		newNote->m_type = MidiAnalysisNote::NOTE_PLAYED;
		newNote->m_timingError = (double)noteInformation->m_timingError;

		if (!noteInformation->m_correctNote)
		{

			if (!noteInformation->m_nonsenseNote)
			{

				newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_WRONG;
				stats->WrongNotes++;

			}

			else
			{

				newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_NONSENSE;
				stats->NonsenseNotes++;

			}

		}

		else
		{

			MidiNote *referenceNote = noteInformation->m_noteMatch->m_reference;

			newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_NONE;

			newNote->m_realTimeError = (double)
				((int)(newNote->m_realTime / 1000) - (int)(m_startReferenceRealTime / 1000) - referenceNote->GetTimeStampMilliseconds(0, m_core->GetTempoBPM()));

			stats->CorrectNotes++;

			timingErrorAccum += noteInformation->m_timingError;
			timingErrorSamples++;

		}

	}

	if (timingErrorSamples > 0)
		report->m_latencyCorrection = (double)timingErrorAccum / timingErrorSamples;

	else
		report->m_latencyCorrection = 0.0;

	report->m_latencyCorrected = false;

	// Copy the reference notes

	// TODO: put left hand and right hand logic in a function to reduce code duplication

	/* Left Hand */
	int nLeftHandNotes = m_leftHandMatch.GetNumObjects();

	for (int i = 0; i < nLeftHandNotes; i++)
	{

		MidiAnalysisNote *newNote = report->m_referenceSegment.m_leftHand.AddAnalysisNote();
		MidiNote *referenceNote = m_leftHandMatch.Get(i)->m_reference;
		MidiNote *playedNote = m_leftHandMatch.Get(i)->m_played;

		newNote->Copy(referenceNote);

		if (playedNote == NULL)
		{

			newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_MISSED;
			report->m_leftHandStats.MissedNotes++;

		}

		else
		{

			if (playedNote->m_midiKey != referenceNote->m_midiKey)
			{

				newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_WRONG;
				
				// Note: wrong notes are already counted by this point

			}

			else
				newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_NONE;

		}

	}

	/* Right Hand */
	int nRightHandNotes = m_rightHandMatch.GetNumObjects();

	for (int i = 0; i < nRightHandNotes; i++)
	{

		MidiAnalysisNote *newNote = report->m_referenceSegment.m_rightHand.AddAnalysisNote();
		MidiNote *referenceNote = m_rightHandMatch.Get(i)->m_reference;
		MidiNote *playedNote = m_rightHandMatch.Get(i)->m_played;

		newNote->Copy(referenceNote);

		if (playedNote == NULL)
		{

			newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_MISSED;
			report->m_rightHandStats.MissedNotes++;

		}

		else
		{

			if (playedNote->m_midiKey != referenceNote->m_midiKey)
			{

				newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_WRONG;

				// Note: wrong notes are already counted by this point

			}

			else
				newNote->m_errorType = MidiAnalysisNote::NOTE_ERROR_NONE;

		}

	}

	// Correct for latency in the system
	report->CorrectLatency();

}

void Toccata_PatternDetector::ResolveNonsenseNoteHand()
{

	// Assign a hand to the played notes that don't have hands
	int nNotes = m_noteInformation.GetNumObjects();

	for (int i = 0; i < nNotes; i++)
	{

		Toccata_NoteInformation *noteInformation = m_noteInformation.Get(i);

		MidiNote *note = noteInformation->m_playedNote;
		Toccata_NoteInformation::HAND hand = noteInformation->m_hand;

		if (hand == Toccata_NoteInformation::HAND_UNKNOWN)
		{

			Toccata_NoteInformation *closest = FindNearestHandHint(i);
			noteInformation->m_hand = closest->m_hand;

		}

	}

}

Toccata_NoteInformation *Toccata_PatternDetector::FindNearestHandHint(int index)
{

	// Find the closest note that has a known hand
	int nNotes = m_noteInformation.GetNumObjects();

	Toccata_NoteInformation *closestNote = NULL;
	int closestTimeDiff = 0;
	int closestKeyDiff = 0;

	MidiNote *ref = m_noteInformation.Get(index)->m_playedNote;

	for (int i = 0; i < nNotes; i++)
	{

		Toccata_NoteInformation *noteInformation = m_noteInformation.Get(i);

		MidiNote *note = noteInformation->m_playedNote;
		Toccata_NoteInformation::HAND hand = noteInformation->m_hand;

		int timeDiff = abs(note->m_timeStamp - ref->m_timeStamp);
		int keyDiff = abs(note->m_midiKey - ref->m_midiKey);

		if (hand != Toccata_NoteInformation::HAND_UNKNOWN)
		{

			if (closestNote == NULL)
			{

				closestNote = noteInformation;
				closestTimeDiff = timeDiff;
				closestKeyDiff = keyDiff;

			}

			else
			{

				if (keyDiff < 5)
				{

					if (timeDiff < closestTimeDiff)
					{

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