#include "../include/recorder.h"

#include "../include/core.h"

toccata::InputBuffer::InputBuffer() {
    /* void */
}

toccata::InputBuffer::~InputBuffer() {
    /* void */
}

toccata::RawMidiFileOutput::RawMidiFileOutput() {
    /* void */
}

toccata::RawMidiFileOutput::~RawMidiFileOutput() {
    /* void */
}

void toccata::RawMidiFileOutput::Open(const char *fname) {
    m_file.open(fname, std::ios::out);
}

void toccata::RawMidiFileOutput::WriteLine(DWORD wMsg, DWORD dwParam1, DWORD dwParam2) {
    m_file << wMsg << " ";
    m_file << dwParam1 << " ";
    m_file << dwParam2 << "\n";
}

toccata::RawMidiFileInput::RawMidiFileInput() {
    /* void */
}

toccata::RawMidiFileInput::~RawMidiFileInput() {
    /* void */
}

bool toccata::RawMidiFileInput::ReadLine(DWORD *wMsg, DWORD *dwParam1, DWORD *dwParam2) {
    if (m_file.eof()) return false;

    m_file >> *wMsg;
    m_file >> *dwParam1;
    m_file >> *dwParam2;

    return true;
}

void toccata::RawMidiFileInput::Open(const char *fname) {
    m_file.open(fname, std::ios::in);
}

toccata::Recorder::Recorder() {
    // Create the first recording
    NewSegment();

    m_tempoBPM = 0;
    m_isMetronomeEnabled = false;
    m_meterNumerator = 0;
    m_meterDenominator = 0;

    m_initialized = false;
}

toccata::Recorder::~Recorder() {
    /* void */
}

void toccata::Recorder::Initialize() {
    std::string fname = toccata::Core::Get()->GetDirectory();
    fname += "/raw_recording.rmid";

    m_rawOutput.Open(fname.c_str());

    m_initialized = true;
}

toccata::Recorder::ErrorCode toccata::Recorder::OnNewTempo(
    int tempoBPM, int meterNumerator, int meterDenominator, bool metronomeEnabled) 
{
    MidiPianoSegment *newSegment = nullptr;

    m_tempoBPM = tempoBPM;
    m_meterNumerator = meterNumerator;
    m_meterDenominator = meterDenominator;
    m_isMetronomeEnabled = metronomeEnabled;

    if (m_currentTarget->IsEmpty()) {
        // Just reuse the current segment
        newSegment = m_currentTarget;
        m_midiTimeStampStart = 0;
        m_currentMidiTimeStamp = 0;
        m_currentMidiTimeStampExternal = 0;
        m_recording = false;
    }
    else {
        // Create a new segment
        newSegment = NewSegment();
        m_midiTimeStampStart = 0;
        m_currentMidiTimeStamp = 0;
        m_currentMidiTimeStampExternal = 0;
        m_recording = false;
    }

    newSegment->SetTimeFormat(MidiPianoSegment::TimeFormat::TimeCode);

    if (m_tempoBPM == 0 || !metronomeEnabled) {
        // Assume 120 bpm, 4/4 time
        // Correct values will be calculated below to allow
        // for millisecond accuracy of recordings

        m_tempoBPM = 120;
        m_meterNumerator = 4;
        m_meterDenominator = 2;
        m_isMetronomeEnabled = false;
    }
    
    newSegment->SetTempoBPM(m_tempoBPM);

    // Calculate number of ticks needed for millisecond accuracy
    double period = (60.0 / m_tempoBPM) / 4;
    period *= 1000; // Convert to ms

    newSegment->SetTicksPerQuarterNote((int)(period + 0.5) * 100);
    newSegment->SetTimeSignature(m_meterNumerator, m_meterDenominator);

    return ErrorCode::None;
}

toccata::Recorder::ErrorCode toccata::Recorder::OnNewTime(
    int tempoBPM, int meterNumerator, int meterDenominator, bool metronomeEnabled) 
{
    return OnNewTempo(tempoBPM, meterNumerator, meterDenominator, metronomeEnabled);
}

void toccata::Recorder::IncrementTime(double dt) {
    if (m_recording) {
        //m_deltaTime += m_currentTarget->ConvertSecondsToDeltaTime(dt, m_currentTarget->GetTempo());
    }
}

int toccata::Recorder::OutstandingNoteCount() {
    int nNotes = m_inputBuffer.GetTrack()->GetNoteCount();
    int outstandingCount = 0;

    for (int i = 0; i < nNotes; i++) {
        if (!m_inputBuffer.GetNotes()[i]->GetValid()) outstandingCount++;
    }

    return outstandingCount;
}

int toccata::Recorder::ProcessInputBuffer(int *nOutstanding) {
    m_inputBuffer.LockBuffer();

    int nNotes = m_inputBuffer.GetTrack()->GetNoteCount();
    MidiNote **notes;

    int newNotes = 0;

    for (int i = 0; i < nNotes; i++) {
        // The notes array MAY be resized in this loop
        notes = m_inputBuffer.GetTrack()->GetBuffer();

        if (notes[i]->GetValid()) {
            if (notes[i]->m_timeStamp > m_currentMidiTimeStamp) {
                m_currentMidiTimeStamp = notes[i]->m_timeStamp;
            }

            //notes[i]->PrintDebug(m_core->GetTempo());

            // Transfer the note to the right hand track
            m_inputBuffer.GetTrack()->TransferNote(notes[i], &m_currentTarget->m_rightHand);

            // One note has been removed from the input buffer track
            i--;
            nNotes--;
            newNotes++;
        }
        else {
            // Nothing after this point is valid
            break;
        }
    }

    m_currentMidiTimeStampExternal = m_currentMidiTimeStamp;

    // Get the number of outstanding notes before the buffer is unlocked
    *nOutstanding = OutstandingNoteCount();

    m_inputBuffer.UnlockBuffer();

    // Return the number of new notes to process
    return newNotes;
}

void toccata::Recorder::ProcessMidiTick(int timeStamp) {
    m_inputBuffer.LockBuffer();

    int relativeTimeStamp = timeStamp - m_midiTimeStampStart;

    if (OutstandingNoteCount() == 0) {
        relativeTimeStamp = m_currentTarget->ConvertMillisecondsToDeltaTime(relativeTimeStamp, m_currentTarget->GetTempoBPM());

        if (relativeTimeStamp > m_currentMidiTimeStamp) {
            m_currentMidiTimeStamp = relativeTimeStamp;
        }
    }
    else {
        // Future cannot be reasonably declared
    }

    m_inputBuffer.UnlockBuffer();
}

toccata::Recorder::ErrorCode toccata::Recorder::ProcessEvent(
    int midiKey, int velocity, int timeStamp, uint64_t systemTimeStamp) 
{
    if (m_tempoBPM == 0)			return ErrorCode::InvalidInternalState;
    if (m_meterNumerator == 0)		return ErrorCode::InvalidInternalState;
    if (m_meterDenominator == 0)	return ErrorCode::InvalidInternalState;

    m_inputBuffer.LockBuffer();

    // If this is the first key-press, then start the recording
    if (m_currentTarget->IsEmpty() && velocity > 0) {
        m_midiTimeStampStart = timeStamp;
        m_recording = true;
    }

    int relativeTimeStamp = timeStamp - m_midiTimeStampStart;

    if (velocity > 0) {
        // Note was pressed

        MidiNote *newNote = m_inputBuffer.NewNote();

        newNote->m_realTime = systemTimeStamp;
        newNote->m_timeStamp = m_currentTarget->ConvertMillisecondsToDeltaTime(relativeTimeStamp, m_currentTarget->GetTempoBPM());
        newNote->m_midiKey = midiKey;
        newNote->m_velocity = velocity;
        newNote->SetValid(false);
    }
    else {
        // Note was released

        int currentTime = m_currentTarget->ConvertMillisecondsToDeltaTime(relativeTimeStamp, m_currentTarget->GetTempoBPM());
        MidiNote *lastNote = m_inputBuffer.GetTrack()->FindLastNote(midiKey, currentTime);

        if (lastNote != NULL) {
            lastNote->m_noteLength = m_currentTarget->ConvertMillisecondsToDeltaTime(relativeTimeStamp, m_currentTarget->GetTempoBPM()) - lastNote->m_timeStamp;
            lastNote->SetValid(true);
        }
        else {
            // This was likely a key released before this track began recording
        }
    }

    m_inputBuffer.UnlockBuffer();

    return ErrorCode::None;
}

toccata::MidiPianoSegment *toccata::Recorder::NewSegment() {
    MidiPianoSegment *newSegment = m_recordedSegments.New();

    m_currentTarget = newSegment;
    m_midiTimeStampStart = 0;

    m_inputBuffer.LockBuffer();
    m_inputBuffer.GetTrack()->Clear();
    m_inputBuffer.GetTrack()->SetSegment(newSegment);
    m_inputBuffer.UnlockBuffer();

    return newSegment;
}
