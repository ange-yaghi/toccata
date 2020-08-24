#include "../include/basic_solve_benchmark.h"
#include "../include/midi_device_testbench.h"

int main() {
    toccata::MidiDeviceTestbench benchmark;
    benchmark.Run();

    return 0;
}
