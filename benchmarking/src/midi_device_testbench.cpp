#include "../include/midi_device_testbench.h"

#include "../../include/midi_handler.h"
#include "../../include/midi_device_system.h"

#include <iostream>

toccata::MidiDeviceTestbench::MidiDeviceTestbench() {
    m_lastLength = 0;
}

toccata::MidiDeviceTestbench::~MidiDeviceTestbench() {
    /* void */
}

void toccata::MidiDeviceTestbench::Run() {
    MidiDeviceSystem system;
    system.Refresh();

    const bool connectSuccess = system.Connect(1);

    if (connectSuccess) {
        std::wcout << "Successfully connected to " << system.GetLastDeviceName() << "\n";
    }
    else {
        std::wcout << "Failed to connect\n";
    }

    InitializeMidiInput();

    while (true) {
        const bool connected = system.IsConnected();

        while (true) {
            const bool status = system.Refresh();
            if (!status) {
                if (system.GetLastErrorCode() == MidiDeviceSystem::ErrorCode::DeviceListChangedDuringUpdate) {
                    continue;
                }
                else {
                    std::wcout << "Could not refresh device list\n";
                }
            }
            else break;
        }

        if (connected && !system.IsConnected()) {
            std::wcout << "Disconnected from " << system.GetLastDeviceName() << "\n";
        }

        if (!system.IsConnected()) {
            const bool reconnected = system.Reconnect();
            if (reconnected) {
                std::wcout << "Reconnected to " << system.GetLastDeviceName() << "\n";
            }
        }

        ProcessMidiInput();
    }
}

void toccata::MidiDeviceTestbench::InitializeMidiInput() {
    
}

void toccata::MidiDeviceTestbench::ProcessMidiInput() {
    MidiStream targetStream;
    MidiHandler::Get()->Extract(&targetStream);

    const int noteCount = targetStream.GetNoteCount();
    for (int i = 0; i < noteCount; ++i) {
        const MidiNote &note = targetStream.GetNote(i);
        std::cout << "{ key=" << note.MidiKey << ", vel=" << note.Velocity << ", len=" << note.NoteLength << " }\n";
    }
}
