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
#include "metrics_panel.h"

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
        void MockMidiKey(ysKey::Code key, int midiKey);

        void InitializeLibrary();
        void InitializeDecisionThread();
        void InitializeMidiInput();
        void ProcessMidiInput();
        void CheckMidiStatus();
        void ConstructReferenceNotes();

        void ReloadThemes();

        dbasic::DeltaEngine m_engine;
        dbasic::AssetManager m_assetManager;
        dbasic::ShaderSet m_shaderSet;
        dbasic::DefaultShaders m_shaders;

        dbasic::TextRenderer m_textRenderer;

        MetricsPanel m_metricsPanel;
        BarDisplay m_barDisplay;
        MidiDisplay m_midiDisplay;
        PieceDisplay m_pieceDisplay;
        MusicSegment m_testSegment;
        MusicSegment m_unresolvedNotes;
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

        bool *m_isKeydown;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_APPLICATION_H */
