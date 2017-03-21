#include <boost/test/unit_test.hpp>

#include <toccata_session.h>
#include <toccata_recorder.h>

BOOST_AUTO_TEST_SUITE(Toccata_RecorderSanityCheck);

void GenerateEvent(Toccata_Recorder *recorder, int midiKey, int velocity, int timeStamp)
{

	Toccata_Recorder::ERROR_CODE error;

	error = recorder->ProcessEvent(midiKey, velocity, timeStamp, timeStamp);
	BOOST_TEST(error == Toccata_Recorder::ERROR_NONE);

}

BOOST_AUTO_TEST_CASE(SanityCheck_1_0)
{

	Toccata_Recorder recorder;

	// Check that the variables are initialized correctly
	BOOST_TEST(recorder.GetMeterNumerator() == 0);
	BOOST_TEST(recorder.GetMeterDenominator() == 0);
	BOOST_TEST(recorder.GetTempoBPM() == 0);
	BOOST_TEST(recorder.IsMetronomeEnabled() == false);

	recorder.OnNewTempo(100, 4, 2, true);
	BOOST_TEST(recorder.GetMeterNumerator() == 4);
	BOOST_TEST(recorder.GetMeterDenominator() == 2);
	BOOST_TEST(recorder.GetTempoBPM() == 100);
	BOOST_TEST(recorder.IsMetronomeEnabled() == true);

	recorder.OnNewTempo(0, 3, 4, true);
	BOOST_TEST(recorder.GetMeterNumerator() == 4);
	BOOST_TEST(recorder.GetMeterDenominator() == 2);
	BOOST_TEST(recorder.GetTempoBPM() == 120);
	BOOST_TEST(recorder.IsMetronomeEnabled() == false);

	recorder.OnNewTempo(100, 3, 4, false);
	BOOST_TEST(recorder.GetMeterNumerator() == 4);
	BOOST_TEST(recorder.GetMeterDenominator() == 2);
	BOOST_TEST(recorder.GetTempoBPM() == 120);
	BOOST_TEST(recorder.IsMetronomeEnabled() == false);

	recorder.OnNewTempo(110, 3, 4, true);
	BOOST_TEST(recorder.GetMeterNumerator() == 3);
	BOOST_TEST(recorder.GetMeterDenominator() == 4);
	BOOST_TEST(recorder.GetTempoBPM() == 110);
	BOOST_TEST(recorder.IsMetronomeEnabled() == true);

	recorder.OnNewTempo(100, 3, 4, false);
	BOOST_TEST(recorder.GetMeterNumerator() == 4);
	BOOST_TEST(recorder.GetMeterDenominator() == 2);
	BOOST_TEST(recorder.GetTempoBPM() == 120);
	BOOST_TEST(recorder.IsMetronomeEnabled() == false);

}

BOOST_AUTO_TEST_CASE(SanityCheck_Initialize)
{

	Toccata_Recorder recorder;

	BOOST_TEST(recorder.IsInitialized() == false);

	recorder.Initialize();

	BOOST_TEST(recorder.IsInitialized() == true);

}

BOOST_AUTO_TEST_CASE(SanityCheck_UnitializedRun)
{

	Toccata_Recorder recorder;
	Toccata_Recorder::ERROR_CODE error;
	int noteCount;

	recorder.Initialize();
	error = recorder.ProcessEvent(10, 10, 0, 0);

	BOOST_TEST(error == Toccata_Recorder::ERROR_INVALID_INTERNAL_STATE);

	noteCount = recorder.OutstandingNoteCount();

	BOOST_TEST(noteCount == 0);

}

BOOST_AUTO_TEST_CASE(UnitTest_ProcessSingleNote)
{

	const int MIDI_KEY = 10;
	const int NOTE_VELOCITY = 10;
	const int NOTE_LENGTH = 10;
	const int NOTE_START = 1;

	Toccata_Recorder recorder;
	Toccata_Recorder::ERROR_CODE error;
	MidiNote *newNote;
	int unresolvedNotes = 0;
	int noteCount = 0;

	recorder.Initialize();
	recorder.OnNewTempo(120, 4, 2, true);

	error = recorder.ProcessEvent(MIDI_KEY, NOTE_VELOCITY, NOTE_START, NOTE_START);
	BOOST_TEST(error == Toccata_Recorder::ERROR_NONE);

	unresolvedNotes = recorder.OutstandingNoteCount();
	BOOST_TEST(unresolvedNotes == 1);

	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	BOOST_TEST(noteCount == 1);

	newNote = recorder.GetInputBuffer()->GetNotes()[0];

	BOOST_TEST(newNote->m_midiKey == MIDI_KEY);
	BOOST_TEST(newNote->m_noteLength == 0);
	BOOST_TEST(newNote->m_velocity == NOTE_VELOCITY);
	BOOST_TEST(newNote->m_realTime == NOTE_START);
	BOOST_TEST(newNote->m_timeStamp != 0);
	BOOST_TEST(newNote->GetValid() == false);

	error = recorder.ProcessEvent(MIDI_KEY, 0, NOTE_START + NOTE_LENGTH, NOTE_START + NOTE_LENGTH);
	BOOST_TEST(error == Toccata_Recorder::ERROR_NONE);

	unresolvedNotes = recorder.OutstandingNoteCount();
	BOOST_TEST(unresolvedNotes == 0);

	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	BOOST_TEST(noteCount == 1);

	newNote = recorder.GetInputBuffer()->GetNotes()[0];

	BOOST_TEST(newNote->m_midiKey == MIDI_KEY);
	BOOST_TEST(newNote->m_noteLength != 0);
	BOOST_TEST(newNote->m_velocity == NOTE_VELOCITY);
	BOOST_TEST(newNote->m_realTime == NOTE_START);
	BOOST_TEST(newNote->m_timeStamp != 0);
	BOOST_TEST(newNote->GetValid() == true);

}

BOOST_AUTO_TEST_CASE(UnitTest_ProcessTwoNotes)
{

	Toccata_Recorder recorder;
	Toccata_Recorder::ERROR_CODE error;
	MidiNote *newNote1;
	MidiNote *newNote2;
	int unresolvedNotes = 0;
	int noteCount = 0;

	recorder.Initialize();
	recorder.OnNewTempo(120, 4, 2, true);

	GenerateEvent(&recorder, 0, 10, 0);
	GenerateEvent(&recorder, 1, 10, 1);

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	BOOST_TEST(unresolvedNotes == 2);
	BOOST_TEST(noteCount == 2);
	
	GenerateEvent(&recorder, 0, 0, 20);
	GenerateEvent(&recorder, 1, 0, 20);

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	BOOST_TEST(unresolvedNotes == 0);
	BOOST_TEST(noteCount == 2);

	newNote1 = recorder.GetInputBuffer()->GetNotes()[0];
	newNote2 = recorder.GetInputBuffer()->GetNotes()[1];

	BOOST_TEST(newNote1->m_midiKey == 0);
	BOOST_TEST(newNote2->m_midiKey == 1);

}

BOOST_AUTO_TEST_CASE(UnitTest_Process100Notes)
{

	Toccata_Recorder recorder;
	Toccata_Recorder::ERROR_CODE error;
	MidiNote *newNote;
	int unresolvedNotes = 0;
	int noteCount = 0;

	recorder.Initialize();
	recorder.OnNewTempo(120, 4, 2, true);

	for (int i = 0; i < 100; i++)
	{

		GenerateEvent(&recorder, i, 10, 0 + i);

	}

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	BOOST_TEST(unresolvedNotes == 100);
	BOOST_TEST(noteCount == 100);

	for (int i = 0; i < 100; i++)
	{

		GenerateEvent(&recorder, i, 0, 2000);

	}

	unresolvedNotes = recorder.OutstandingNoteCount();
	noteCount = recorder.GetInputBuffer()->GetTrack()->GetNoteCount();
	BOOST_TEST(unresolvedNotes == 0);
	BOOST_TEST(noteCount == 100);

	for (int i = 0; i < 100; i++)
	{

		newNote = recorder.GetInputBuffer()->GetNotes()[i];

		BOOST_TEST(newNote->m_midiKey == i);

	}

}

BOOST_AUTO_TEST_SUITE_END()