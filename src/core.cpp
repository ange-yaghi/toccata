#include "../include/core.h"

#include "../include/set_tempo_fsm.h"
#include "../include/set_time_fsm.h"
#include "../include/performance_report.h"
#include "../include/midi_callback.h"

const char *toccata::Core::Line = "----------------------------------------\n";
toccata::Core *toccata::Core::s_mainCore = nullptr;

toccata::Core::Core() {
    m_setTempoFSM = NULL;
    m_setTimeFSM = NULL;

    m_metronomeEnabled = false;
    m_currentTempo = 120;
    m_currentTime = 4;

    m_systemTime = 0;
    m_currentTimer = 0;

    m_metronomeOffset = 0;
    m_metronomeTicks = 0;

    m_mode = Mode::FileSource;
}

toccata::Core::~Core() {
    /* void */
}

toccata::Core *toccata::Core::Get() {
    if (s_mainCore == nullptr) {
        s_mainCore = new Core();
    }

    return s_mainCore;
}

void toccata::Core::Initialize() {
    CreateFsms();

    m_timingSystem.Initialize();

    m_patternDetector.SetCore(this);
}

void toccata::Core::Update() {
    // Check each fsm

    // Check the tempo fsm
    m_setTempoFSM->Lock();
    if (m_setTempoFSM->IsStateChanged()) {

        SetTempo(m_setTempoFSM->GetTempo());

        if (m_setTempoFSM->GetForceMetronomeEnable()) {
            EnableMetronome(true);
        }
        else if (m_setTempoFSM->GetSwitchMetronomeOnOff()) {
            EnableMetronome(!IsMetronomeEnabled());
        }
    }
    m_setTempoFSM->OnUpdate();
    m_setTempoFSM->ClearFlag();
    m_setTempoFSM->Unlock();

    // Check the time fsm
    m_setTimeFSM->Lock();
    if (m_setTimeFSM->IsStateChanged()) {

        SetTime(m_setTimeFSM->GetCurrentMeter());

        if (m_setTimeFSM->GetMetronomeEnabled()) {
            EnableMetronome(!IsMetronomeEnabled());
        }
    }

    m_setTimeFSM->OnUpdate();
    m_setTimeFSM->ClearFlag();
    m_setTimeFSM->Unlock();

    // Update the timing system
    m_timingSystem.Update();
}

void toccata::Core::ResetMetronome() {
    if (m_currentTempo != 0) {
        m_currentTimer = (unsigned int)(m_currentTempo);
    }

    m_metronomeTicks = 0;

    // Instead of setting is to 0, the system time is used
    // which is a good approximation
    m_metronomeOffset = m_systemTime;
}

void toccata::Core::SetTempo(int tempoBPM) {
    double tempoPeriod = (double)(60000000 / tempoBPM);
    int newTempo = (int)(tempoPeriod + 0.5);

    if (newTempo != m_currentTempo) {
        m_currentTempo = newTempo;

        m_recorder.OnNewTempo(tempoBPM, m_currentTime, 2, true);
    }
}

int toccata::Core::GetTempoBPM() const {
    double intermediate = m_currentTempo / 1000.0;

    int tempoBPM = (int)(60000.0 / intermediate);

    return tempoBPM;
}

void toccata::Core::SetTime(int time) {
    if (time != m_currentTime) {
        m_currentTime = time;
        m_recorder.OnNewTime(GetTempoBPM(), m_currentTime, 2, true);
    }
}

void toccata::Core::EnableMetronome(bool enable) {
    if (enable != m_metronomeEnabled) {
        m_metronomeEnabled = enable;

        ResetMetronome();

        m_recorder.OnNewTempo(GetTempoBPM(), m_currentTime, 2, true);
    }
}

void toccata::Core::AddExercise(const char *name, const char *midiReferenceFile) {
    MidiFile midiFile;
    MidiPianoSegment *segment = midiFile.Read(midiReferenceFile);
    Exercise *newExercise = m_session.NewExercise(name, segment);

    m_patternDetector.NewSegment(segment, newExercise);
}

int toccata::Core::NewPerformanceID() {
    return m_database.NewPerformanceID();
}

void toccata::Core::CreateFsms() {
    m_setTempoFSM = NewFsm<toccata::SetTempoFsm>();
    m_setTimeFSM = NewFsm<toccata::SetTimeFsm>();
}

void toccata::Core::MainLoop() {
    m_database.OpenDatabase();

    AddExercise("Minuet I Section I", "Reference Music/Minuet I/MinuetI_Section1.mid");
    AddExercise("Minuet I Section II", "Reference Music/Minuet I/MinuetI_Section2.mid");

    AddExercise("Minuet II Section I", "Reference Music/Minuet II/MinuetII_Section1.mid");
    AddExercise("Minuet II Section II", "Reference Music/Minuet II/MinuetII_Section2.mid");

    AddExercise("Minuet III Section I", "Reference Music/Minuet III/MinuetIII_Section1.mid");
    AddExercise("Minuet III Section II", "Reference Music/Minuet III/MinuetIII_Section2.mid");

    AddExercise("Burgmuller Etude 2 Section I", "Reference Music/Burgmuller/L'Arabesque/L'Arabesque_Section1.mid");

    AddExercise("Burgmuller Etude 5 Section I", "Reference Music/Burgmuller/Innocence/Innocence_Section1.mid");
    AddExercise("Burgmuller Etude 5 Section II", "Reference Music/Burgmuller/Innocence/Innocence_Section2.mid");

    AddExercise("C Major Scale", "Reference Music/Scales/Major/c_major_scale.mid");
    AddExercise("C# Major Scale", "Reference Music/Scales/Major/c_sharp_major_scale.mid");
    AddExercise("D Major Scale", "Reference Music/Scales/Major/d_major_scale.mid");
    AddExercise("Eb Major Scale", "Reference Music/Scales/Major/e_flat_major_scale.mid");
    AddExercise("E Major Scale", "Reference Music/Scales/Major/e_major_scale.mid");
    AddExercise("F Major Scale", "Reference Music/Scales/Major/f_major_scale.mid");
    AddExercise("F# Major Scale", "Reference Music/Scales/Major/f_sharp_major_scale.mid");

    // TEMP
    m_session.SetDirectory("Records");

    // Create a session directory for this session
    m_session.CreateSessionDirectory();

    // Initialize the recorder
    m_recorder.Initialize();

    // Enable the tempo FSM
    m_setTempoFSM->SetEnable(true);
    m_setTimeFSM->SetEnable(true);

    m_currentTimer = 0;

    Sound *highSeiko = m_soundSystem.CreateSound("Assets/Downloads/SeikoSQ50/High Seiko SQ50.wav");
    Sound *lowSeiko = m_soundSystem.CreateSound("Assets/Downloads/SeikoSQ50/Low Seiko SQ50.wav");

    int timeSignatureN = m_currentTime;
    int timeSignatureD = 4;

    int beat = 0;

    MidiFile debugInputFile;
    MidiPianoSegment *debugInput = debugInputFile.Read("debug.mid");

    bool readInFile = false;

    while (true) {
        Update();

        // Process the input buffer
        int nOutstanding;
        int newNotes = m_recorder.ProcessInputBuffer(&nOutstanding);

        int nNotes = m_recorder.m_currentTarget->m_rightHand.GetNoteCount();

        for (int i = nNotes - newNotes; i < nNotes; i++) {
            MidiNote *note = m_recorder.m_currentTarget->m_rightHand.GetBuffer()[i];
            m_patternDetector.ProcessNote(note);
        }

        if (newNotes == 0 && nOutstanding <= 0) {
            m_patternDetector.Synchronize(&m_recorder.m_currentTarget->m_rightHand, m_recorder.GetCurrentMidiTimeStamp());
        }

        if (m_mode == Mode::FileSource) {
            if (!readInFile) {
                RawMidiFileInput recordedSession;
                recordedSession.Open("raw_recording.rmid");

                bool result = true;
                while (result) {
                    DWORD wMsg;
                    DWORD dwParam1;
                    DWORD dwParam2;

                    result = recordedSession.ReadLine(&wMsg, &dwParam1, &dwParam2);

                    if (!result) break;
                    else Toccata_MIDI_Callback(NULL, wMsg, NULL, dwParam1, dwParam2);

                    // This call is necessary to update things like the internal tempo
                    // since otherwise the tempo would not be updated when real notes start
                    // coming in
                    Update();
                }
            }

            readInFile = true;
        }

        m_patternDetector.Update();

        m_systemTime += m_timingSystem.GetFrameDuration_us();

        // Temporary
        if (m_currentTempo > 0 && m_metronomeEnabled) {
            m_currentTimer += m_timingSystem.GetFrameDuration_us();

            if (m_currentTimer >= m_currentTempo) {
                if (m_metronomeTicks == 0) m_metronomeOffset = m_timingSystem.GetTime();
                m_metronomeTicks++;

                m_currentTimer = m_currentTimer % m_currentTempo;

                if (beat == 0) {
                    highSeiko->Play();
                }
                else {
                    lowSeiko->Play();
                }

                beat = (beat + 1) % m_currentTime;
            }
        }
        else {
            // Reset the metronome
            m_currentTimer = 0;
            beat = 0;
        }
    }
}
