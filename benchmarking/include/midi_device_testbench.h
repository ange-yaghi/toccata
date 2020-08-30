#ifndef TOCCATA_BENCHMARKING_MIDI_DEVICE_TESTBENCH_H
#define TOCCATA_BENCHMARKING_MIDI_DEVICE_TESTBENCH_H

#include "benchmarking_test.h"

#include "../../include/midi_stream.h"
#include "../../include/decision_thread.h"

namespace toccata {

    class MidiDeviceTestbench : public BenchmarkingTest {
    public:
        MidiDeviceTestbench();
        ~MidiDeviceTestbench();

        virtual void Run();

        void InitializeDecisionThread();
        void InitializeMidiInput();
        void ProcessMidiInput();

    protected:
        int m_lastLength;

        Library m_library;
        DecisionThread m_decisionThread;
    };

} /* namespace toccata */

#endif /* TOCCATA_BENCHMARKING_MIDI_DEVICE_TESTBENCH_H */
