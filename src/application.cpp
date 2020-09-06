#include "../include/application.h"

#include "../include/midi_stream.h"
#include "../include/midi_handler.h"
#include "../include/midi_file.h"
#include "../include/segment_generator.h"
#include "../include/transform.h"
#include "../include/midi_callback.h"

#include <sstream>

toccata::Application::Application() {
    m_currentOffset = 0.0;
}

toccata::Application::~Application() {
    /* void */
}

void toccata::Application::Initialize(void *instance, ysContextObject::DeviceAPI api) {
    dbasic::Path modulePath = dbasic::GetModulePath();
    dbasic::Path confPath = modulePath.Append("delta.conf");

    std::string enginePath = "../../dependencies/delta/engines/basic";
    std::string assetPath = "../../assets";
    if (confPath.Exists()) {
        std::fstream confFile(confPath.ToString(), std::ios::in);
        
        std::getline(confFile, enginePath);
        std::getline(confFile, assetPath);
        enginePath = modulePath.Append(enginePath).ToString();
        assetPath = modulePath.Append(assetPath).ToString();

        confFile.close();
    }

    m_engine.GetConsole()->SetDefaultFontDirectory(enginePath + "/fonts/");

    std::string shaderDirectory = enginePath + "/shaders/";

    dbasic::DeltaEngine::GameEngineSettings settings;
    settings.API = api;
    settings.DepthBuffer = true;
    settings.FrameLogging = false;
    settings.Instance = instance;
    settings.LoggingDirectory = "";
    settings.ShaderDirectory = shaderDirectory.c_str();
    settings.WindowTitle = "Toccata";

    m_engine.CreateGameWindow(settings);
    m_engine.SetCameraMode(dbasic::DeltaEngine::CameraMode::Target);
    m_engine.SetClearColor(ysColor::srgbiToLinear(0x00, 0x00, 0x00));

    m_assetManager.SetEngine(&m_engine);

    m_analyzer.SetTimeline(&m_timeline);

    m_barDisplay.Initialize(&m_engine);
    m_barDisplay.SetTextRenderer(&m_textRenderer);
    m_barDisplay.SetAnalyzer(&m_analyzer);

    m_midiDisplay.SetAnalyzer(&m_analyzer);

    m_textRenderer.SetEngine(&m_engine);
    m_textRenderer.SetRenderer(m_engine.GetUiRenderer());
    m_textRenderer.SetFont(m_engine.GetConsole()->GetFont());

    m_testSegment.PulseUnit = 1000.0;
    m_testSegment.PulseRate = 1.0;
}

void toccata::Application::Process() {
    const int windowWidth = m_engine.GetScreenWidth();
    const int windowHeight = m_engine.GetScreenHeight();

    const float dt = m_engine.GetFrameLength();
    m_currentOffset += dt;

    const int n = m_testSegment.NoteContainer.GetCount();
    timestamp windowStart = m_timeline.GetTimeOffset();
    if (n > 0) {
        const MusicPoint &lastPoint = m_testSegment.NoteContainer.GetPoints()[n - 1];

        if (lastPoint.Timestamp + 2000 > m_timeline.GetTimeRange() + m_timeline.GetTimeOffset()) {
            windowStart = lastPoint.Timestamp + 2000 - 5000;
        }
    }

    m_timeline.SetPositionX(-windowWidth / 2.0f);
    m_timeline.SetInputSegment(&m_testSegment);
    m_timeline.SetTimeOffset(windowStart);
    m_timeline.SetTimeRange(5000);
    m_timeline.SetWidth((float)windowWidth);

    m_midiDisplay.SetEngine(&m_engine);
    m_midiDisplay.SetTextRenderer(&m_textRenderer);
    m_midiDisplay.SetHeight(windowHeight * 0.7f);
    m_midiDisplay.SetKeyRangeStart(0);
    m_midiDisplay.SetKeyRangeEnd(88);
    m_midiDisplay.SetPositionY(windowHeight / 2.0f - windowHeight * 0.2f);
    m_midiDisplay.SetTimeline(&m_timeline);

    m_barDisplay.SetEngine(&m_engine);
    m_barDisplay.SetHeight(windowHeight * 0.2f);
    m_barDisplay.SetMinimumChannelCount(3);
    m_barDisplay.SetPositionY(windowHeight / 2.0f);
    m_barDisplay.SetTextRenderer(&m_textRenderer);
    m_barDisplay.SetTimeline(&m_timeline);

    MockMidiInput();
}

void toccata::Application::Render() {
    m_midiDisplay.Render();
    m_barDisplay.Render();

    std::stringstream ss; 
    ss << "TOCCATA" << "\n";
    ss << m_engine.GetAverageFramerate() << "\n";

    m_engine.GetConsole()->MoveToOrigin();
    m_engine.GetConsole()->DrawGeneralText(ss.str().c_str());
}

void toccata::Application::MockMidiInput() {
    MockMidiKey(ysKeyboard::KEY_A, 48);
    MockMidiKey(ysKeyboard::KEY_S, 50);
    MockMidiKey(ysKeyboard::KEY_D, 52);
    MockMidiKey(ysKeyboard::KEY_F, 53);
    MockMidiKey(ysKeyboard::KEY_G, 55);
    MockMidiKey(ysKeyboard::KEY_H, 57);
}

void toccata::Application::MockMidiKey(ysKeyboard::KEY_CODE key, int midiKey) {
    // Add 100 million years just for fun
    const timestamp t = 
        (timestamp)std::round(m_currentOffset * 1000) + 3153600000000000000;

    if (m_engine.ProcessKeyDown(key)) {
        MidiHandler::Get()->ProcessEvent(0x9, midiKey, 100, t);
    }
    else if (m_engine.ProcessKeyUp(key)) {
        MidiHandler::Get()->ProcessEvent(0x8, midiKey, 0, t);
    }
}

void toccata::Application::ProcessMidiInput() {
    MidiStream targetStream;
    MidiHandler::Get()->Extract(&targetStream);

    const int noteCount = targetStream.GetNoteCount();
    for (int i = 0; i < noteCount; ++i) {
        const MidiNote &note = targetStream.GetNote(i);
        MusicPoint point;
        point.Pitch = note.MidiKey;
        point.Timestamp = note.Timestamp;
        point.Velocity = note.Velocity;
        point.Length = note.NoteLength;
        m_decisionThread.AddNote(point);
        m_testSegment.NoteContainer.AddPoint(point);
    }
}

void toccata::Application::CheckMidiStatus() {
    const bool connected = m_midiSystem.IsConnected();
    const bool status = m_midiSystem.Refresh();
    if (!status) {
        if (m_midiSystem.GetLastErrorCode() == MidiDeviceSystem::ErrorCode::DeviceListChangedDuringUpdate) {

        }
        else {
            // TODO
        }
    }

    if (!connected) {
        const bool reconnected = m_midiSystem.Reconnect();
        if (reconnected) {
            toccata::MidiHandler::Get()->AlignTimestampOffset();
        }
    }

    ProcessMidiInput();
}

void toccata::Application::ConstructReferenceNotes() {
    m_referenceSegment.NoteContainer.Clear();

    m_timeline.ClearBars();

    auto pieces = m_decisionThread.GetPieces();
    for (const DecisionTree::MatchedPiece &piece : pieces) {
        for (const DecisionTree::MatchedBar &bar : piece.Bars) {
            m_timeline.AddBar(bar);

            /*
            MusicSegment *segment = bar.MatchedBar->GetSegment();
            const int n = segment->NoteContainer.GetCount();

            for (int i = 0; i < n; ++i) {
                const MusicPoint &reference = segment->NoteContainer.GetPoints()[i];

                MusicPoint newPoint;
                newPoint.Length = Transform::inv_f(reference.Length, bar.s, 0.0);
                newPoint.Timestamp = Transform::inv_f(reference.Timestamp, bar.s, bar.t);
                newPoint.Pitch = reference.Pitch;
                newPoint.Velocity = reference.Velocity;

                m_referenceSegment.NoteContainer.AddPoint(newPoint);
            }*/
        }
    }

    m_analyzer.Analyze();
}

void toccata::Application::Run() {
    InitializeMidiInput();
    InitializeLibrary();
    InitializeDecisionThread();

    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        CheckMidiStatus();
        ConstructReferenceNotes();

        Process();
        Render();

        m_engine.EndFrame();
    }

    m_decisionThread.KillThreads();
    m_decisionThread.Destroy();
}

void toccata::Application::Destroy() {
    m_assetManager.Destroy();
    m_engine.Destroy();
}

void toccata::Application::InitializeLibrary() {
    const std::string paths[] = 
    {
        "../../test/midi/simple_passage.midi",
        "../../test/midi/simple_passage_2.mid"
    };

    for (const std::string &path : paths) {
        toccata::MidiStream stream;
        toccata::MidiFile midiFile;
        midiFile.Read(path.c_str(), &stream);

        toccata::SegmentGenerator::Convert(&stream, &m_library, 0);
    }
}

void toccata::Application::InitializeDecisionThread() {
    m_decisionThread.Initialize(&m_library, 12, 1000.0, 1.0);
    m_decisionThread.StartThreads();
}

void toccata::Application::InitializeMidiInput() {
    m_midiSystem.Refresh();

    const bool connectSuccess = m_midiSystem.Connect(0);

    if (connectSuccess) {
        // TODO
    }
    else {
        // TODO
    }
}
