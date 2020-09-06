#include "../include/midi_device_testbench.h"

#include "../../include/midi_handler.h"
#include "../../include/midi_device_system.h"
#include "../../include/song_generator.h"

#include <iostream>

toccata::MidiDeviceTestbench::MidiDeviceTestbench() {
    m_lastLength = 0;
}

toccata::MidiDeviceTestbench::~MidiDeviceTestbench() {
    /* void */
}

void toccata::MidiDeviceTestbench::Run() {
    InitializeDecisionThread();

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
                toccata::MidiHandler::Get()->AlignTimestampOffset();
                std::wcout << "Reconnected to " << system.GetLastDeviceName() << "\n";
            }
        }

        ProcessMidiInput();
        std::vector<DecisionTree::MatchedPiece> pieces = m_decisionThread.GetPieces();

        for (DecisionTree::MatchedPiece &piece : pieces) {
            std::cout << "[" << piece.Start << " -> " << piece.End << " // " << piece.Bars.size() << "]";
        }

        std::cout << "                                          \r";
    }
}

void toccata::MidiDeviceTestbench::InitializeDecisionThread() {
    toccata::SongGenerator songGenerator;
}

void toccata::MidiDeviceTestbench::InitializeMidiInput() {
    const std::string path = "../../test/midi/simple_passage.midi";
    const double pulse = 1000.0; // 1 ms pulse

    toccata::MidiStream stream;
    toccata::MidiFile midiFile;
    midiFile.Read(path.c_str(), &stream);

    toccata::SegmentGenerator::Convert(&stream, &m_library, 0);

    m_decisionThread.Initialize(&m_library, 12, pulse, 1.0);
    m_decisionThread.StartThreads();
}

void toccata::MidiDeviceTestbench::ProcessMidiInput() {
    MidiStream targetStream;
    MidiHandler::Get()->Extract(&targetStream);

    const int noteCount = targetStream.GetNoteCount();
    for (int i = 0; i < noteCount; ++i) {
        const MidiNote &note = targetStream.GetNote(i);
        std::cout
            << "{ key=" << note.MidiKey
            << ", vel=" << note.Velocity
            << ", len=" << note.NoteLength
            << ", offset=" << note.Timestamp << " }\n";

        MusicPoint point;
        point.Pitch = note.MidiKey;
        point.Timestamp = note.Timestamp;
        point.Velocity = note.Velocity;
        point.Length = 0;
        m_decisionThread.AddNote(point);
    }
}
