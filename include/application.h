#ifndef TOCCATA_UI_APPLICATION_H
#define TOCCATA_UI_APPLICATION_H

#include "midi_display.h"
#include "bar_display.h"
#include "piece_display.h"

#include "music_segment.h"
#include "midi_device_system.h"
#include "decision_thread.h"
#include "timeline.h"
#include "practice_mode_panel.h"
#include "current_time_display.h"

#include "delta.h"

namespace toccata {

    class Application {
    public:
        Application();
        ~Application();

        void Initialize(void *instance, ysContextObject::DeviceAPI api);
        void Run();
        void Destroy();

    protected:
        void Process();
        void Render();

        void MockMidiInput();
        void MockMidiKey(ysKeyboard::KEY_CODE key, int midiKey);

        void InitializeLibrary();
        void InitializeDecisionThread();
        void InitializeMidiInput();
        void ProcessMidiInput();
        void CheckMidiStatus();
        void ConstructReferenceNotes();

        void ReloadThemes();

        dbasic::DeltaEngine m_engine;
        dbasic::AssetManager m_assetManager;

        dbasic::TextRenderer m_textRenderer;

        BarDisplay m_barDisplay;
        MidiDisplay m_midiDisplay;
        PieceDisplay m_pieceDisplay;
        MusicSegment m_testSegment;
        MusicSegment m_referenceSegment;
        Timeline m_timeline;
        Analyzer m_analyzer;

        MidiDeviceSystem m_midiSystem;
        PracticeModePanel m_practiceModePanel;
        CurrentTimeDisplay m_currentTimeDisplay;

        Library m_library;
        DecisionThread m_decisionThread;

        Settings m_settings;

        float m_currentOffset;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_APPLICATION_H */
