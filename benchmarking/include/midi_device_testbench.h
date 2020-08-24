#ifndef TOCCATA_BENCHMARKING_MIDI_DEVICE_TESTBENCH_H
#define TOCCATA_BENCHMARKING_MIDI_DEVICE_TESTBENCH_H

#include "benchmarking_test.h"

#include "../../include/midi_stream.h"

namespace toccata {

    class MidiDeviceTestbench : public BenchmarkingTest {
    public:
        MidiDeviceTestbench();
        ~MidiDeviceTestbench();

        virtual void Run();

        void InitializeMidiInput();
        void ProcessMidiInput();

    protected:
        int m_lastLength;
    };

} /* namespace toccata */

#endif /* TOCCATA_BENCHMARKING_MIDI_DEVICE_TESTBENCH_H */

