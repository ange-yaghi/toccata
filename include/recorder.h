#ifndef TOCCATA_CORE_RECORDER_H
#define TOCCATA_CORE_RECORDER_H

#include "midi.h"

#include <mutex>

namespace toccata {

    class InputBuffer {
    public:
        InputBuffer();
        ~InputBuffer();

        MidiNote **GetNotes() { return m_noteBuffer.GetBuffer(); }

        void LockBuffer() { m_bufferLock.lock(); }
        void UnlockBuffer() { m_bufferLock.unlock(); }

        MidiNote *NewNote() { return m_noteBuffer.AddNote(); }

        MidiTrack *GetTrack() { return &m_noteBuffer; }

    protected:
        MidiTrack m_noteBuffer;

        std::mutex m_bufferLock;
    };

    class RawMidiFileOutput {
    public:
        RawMidiFileOutput();
        ~RawMidiFileOutput();

        void Open(const char *fname);
        void WriteLine(DWORD wMsg, DWORD dwParam1, DWORD dwParam2);

    protected:
        std::fstream m_file;
    };

    class RawMidiFileInput {
    public:
        RawMidiFileInput();
        ~RawMidiFileInput();

        void Open(const char *fname);
        bool ReadLine(DWORD *wMsg, DWORD *dwParam1, DWORD *dwParam2);

    protected:
        std::fstream m_file;
    };

    class Recorder {
        friend class Core;

    public:
        enum class FsmState {
            Recording,
            PatternCheck
        };

        enum class ErrorCode {
            None,
            InvalidInternalState
        };

    public:
        Recorder();
        ~Recorder();

        void Initialize();

        ErrorCode OnNewTempo(int tempoBPM, int meterNumerator, int meterDenominator, bool metronomeEnabled);
        ErrorCode OnNewTime(int tempoBPM, int meterNumerator, int meterDenominator, bool metronomeEnabled);

        void ProcessMidiTick(int timeStamp);
        ErrorCode ProcessEvent(int midiKey, int velocity, int timeStamp, uint64_t systemTimeStamp);
        void IncrementTime(double dt);

        InputBuffer *GetInputBuffer() { return &m_inputBuffer; }

        int OutstandingNoteCount();
        int ProcessInputBuffer(int *nOutstanding);

        int GetCurrentMidiTimeStamp() const { return m_currentMidiTimeStampExternal; }

        MidiPianoSegment *GetCurrentTarget() const { return m_currentTarget; }

        RawMidiFileOutput *GetRawOutput() { return &m_rawOutput; }

        bool IsMetronomeEnabled() const { return m_isMetronomeEnabled; }
        int GetTempoBPM() { return m_tempoBPM; }
        int GetMeterNumerator() const { return m_meterNumerator; }
        int GetMeterDenominator() const { return m_meterDenominator; }

        bool IsInitialized() const { return m_initialized; }

    protected:
        MidiPianoSegment *NewSegment();

        MidiPianoSegment *m_currentTarget;
        int m_midiTimeStampStart;
        int m_currentMidiTimeStamp;
        int m_currentMidiTimeStampExternal;
        bool m_recording;

    protected:
        ysDynamicArray<MidiPianoSegment, 4> m_recordedSegments;

        InputBuffer m_inputBuffer;

        RawMidiFileOutput m_rawOutput;

    protected:
        // States
        bool m_initialized;

        // Inputs
        bool m_isMetronomeEnabled;
        int m_tempoBPM;
        int m_meterNumerator;
        int m_meterDenominator;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_RECORDER_H */
