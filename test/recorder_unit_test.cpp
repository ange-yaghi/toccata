#include <pch.h>

#include "../include/session.h"
#include "../include/recorder.h"

void GenerateEvent(toccata::Recorder *recorder, int midiKey, int velocity, int timeStamp) {
	toccata::Recorder::ErrorCode error;

	error = recorder->ProcessEvent(midiKey, velocity, timeStamp, timeStamp);
	EXPECT_EQ(error, toccata::Recorder::ErrorCode::None);
}

TEST(RecorderTest, SanityCheck_1_0) {
	toccata::Recorder recorder;

	// Check that the variables are initialized correctly
	EXPECT_EQ(recorder.GetMeterNumerator(), 0);
	EXPECT_EQ(recorder.GetMeterDenominator(), 0);
	EXPECT_EQ(recorder.GetTempoBPM(), 0);
	EXPECT_EQ(recorder.IsMetronomeEnabled(), false);

	recorder.OnNewTempo(100, 4, 2, true);
	EXPECT_EQ(recorder.GetMeterNumerator(), 4);
	EXPECT_EQ(recorder.GetMeterDenominator(), 2);
	EXPECT_EQ(recorder.GetTempoBPM(), 100);
	EXPECT_EQ(recorder.IsMetronomeEnabled(), true);

	recorder.OnNewTempo(0, 3, 4, true);
	EXPECT_EQ(recorder.GetMeterNumerator(), 4);
	EXPECT_EQ(recorder.GetMeterDenominator(), 2);
	EXPECT_EQ(recorder.GetTempoBPM(), 120);
	EXPECT_EQ(recorder.IsMetronomeEnabled(), false);

	recorder.OnNewTempo(100, 3, 4, false);
	EXPECT_EQ(recorder.GetMeterNumerator(), 4);
	EXPECT_EQ(recorder.GetMeterDenominator(), 2);
	EXPECT_EQ(recorder.GetTempoBPM(), 120);
	EXPECT_EQ(recorder.IsMetronomeEnabled(), false);

	recorder.OnNewTempo(110, 3, 4, true);
	EXPECT_EQ(recorder.GetMeterNumerator(), 3);
	EXPECT_EQ(recorder.GetMeterDenominator(), 4);
	EXPECT_EQ(recorder.GetTempoBPM(), 110);
	EXPECT_EQ(recorder.IsMetronomeEnabled(), true);

	recorder.OnNewTempo(100, 3, 4, false);
	EXPECT_EQ(recorder.GetMeterNumerator(), 4);
	EXPECT_EQ(recorder.GetMeterDenominator(), 2);
	EXPECT_EQ(recorder.GetTempoBPM(), 120);
	EXPECT_EQ(recorder.IsMetronomeEnabled(), false);
}

TEST(RecorderTest, SanityCheck_Initialize) {
	toccata::Recorder recorder;

	EXPECT_EQ(recorder.IsInitialized(), false);

	recorder.Initialize();

	EXPECT_EQ(recorder.IsInitialized(), true);
}

TEST(RecorderTest, SanityCheck_UnitializedRun) {
	toccata::Recorder recorder;
	toccata::Recorder::ErrorCode error;
	int noteCount;

	recorder.Initialize();
	error = recorder.ProcessEvent(10, 10, 0, 0);

	EXPECT_EQ(error, toccata::Recorder::ErrorCode::InvalidInternalState);

	noteCount = recorder.OutstandingNoteCount();

	EXPECT_EQ(noteCount, 0);
}

TEST(RecorderTest, UnitTest_ProcessSingleNote) {
	const int MIDI_KEY = 10;
	const int NOTE_VELOCITY = 10;
	const int NOTE_LENGTH = 10;
	const int NOTE_START = 1;

	toccata::Recorder recorder;
	toccata::Recorder::ErrorCode error;
	toccata::MidiNote *newNote;
	int unresolvedNotes = 0;
	int noteCount = 0;

	recorder.Initialize();
	recorder.OnNewTempo(120, 4, 2, true);

	error = recorder.ProcessEvent(MIDI_KEY, NOTE_VELOCITY, NOTE_START, NOTE_START);
	EXPECT_EQ(error, toccata::Recorder::ErrorCode::None);

	unresolvedNotes = recorder.OutstandingNoteCount();
	EXPECT_EQ(unresolvedNotes, 1);

	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	EXPECT_EQ(noteCount, 1);

	newNote = recorder.GetInputBuffer()->GetNotes()[0];

	EXPECT_EQ(newNote->m_midiKey, MIDI_KEY);
	EXPECT_EQ(newNote->m_noteLength, 0);
	EXPECT_EQ(newNote->m_velocity, NOTE_VELOCITY);
	EXPECT_EQ(newNote->m_realTime, NOTE_START);
	EXPECT_NE(newNote->m_timeStamp, 0);
	EXPECT_EQ(newNote->GetValid(), false);

	error = recorder.ProcessEvent(MIDI_KEY, 0, NOTE_START + NOTE_LENGTH, NOTE_START + NOTE_LENGTH);
	EXPECT_EQ(error, toccata::Recorder::ErrorCode::None);

	unresolvedNotes = recorder.OutstandingNoteCount();
	EXPECT_EQ(unresolvedNotes, 0);

	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	EXPECT_EQ(noteCount, 1);

	newNote = recorder.GetInputBuffer()->GetNotes()[0];

	EXPECT_EQ(newNote->m_midiKey, MIDI_KEY);
	EXPECT_NE(newNote->m_noteLength, 0);
	EXPECT_EQ(newNote->m_velocity, NOTE_VELOCITY);
	EXPECT_EQ(newNote->m_realTime, NOTE_START);
	EXPECT_NE(newNote->m_timeStamp, 0);
	EXPECT_EQ(newNote->GetValid(), true);
}

TEST(RecorderTest, UnitTest_ProcessTwoNotes) {
	toccata::Recorder recorder;
	toccata::Recorder::ErrorCode error;
	toccata::MidiNote *newNote1;
	toccata::MidiNote *newNote2;
	int unresolvedNotes = 0;
	int noteCount = 0;

	recorder.Initialize();
	recorder.OnNewTempo(120, 4, 2, true);

	GenerateEvent(&recorder, 0, 10, 0);
	GenerateEvent(&recorder, 1, 10, 1);

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	EXPECT_EQ(unresolvedNotes, 2);
	EXPECT_EQ(noteCount, 2);
	
	GenerateEvent(&recorder, 0, 0, 20);
	GenerateEvent(&recorder, 1, 0, 20);

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	EXPECT_EQ(unresolvedNotes, 0);
	EXPECT_EQ(noteCount, 2);

	newNote1 = recorder.GetInputBuffer()->GetNotes()[0];
	newNote2 = recorder.GetInputBuffer()->GetNotes()[1];

	EXPECT_EQ(newNote1->m_midiKey, 0);
	EXPECT_EQ(newNote2->m_midiKey, 1);
}

TEST(RecorderTest, UnitTest_Process100Notes) {
	toccata::Recorder recorder;
	toccata::Recorder::ErrorCode error;
	toccata::MidiNote *newNote;
	int unresolvedNotes = 0;
	int noteCount = 0;

	recorder.Initialize();
	recorder.OnNewTempo(120, 4, 2, true);

	for (int i = 0; i < 100; i++) {
		GenerateEvent(&recorder, i, 10, 0 + i);
	}

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	EXPECT_EQ(unresolvedNotes, 100);
	EXPECT_EQ(noteCount, 100);

	for (int i = 0; i < 100; i++) {
		GenerateEvent(&recorder, i, 0, 2000);
	}

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	EXPECT_EQ(unresolvedNotes, 0);
	EXPECT_EQ(noteCount, 100);

	for (int i = 0; i < 100; i++) {
		newNote = recorder.GetInputBuffer()->GetNotes()[i];

		EXPECT_EQ(newNote->m_midiKey, i);
	}
}
