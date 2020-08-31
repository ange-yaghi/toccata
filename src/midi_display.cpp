#include "../include/midi_display.h"

#include "../include/transform.h"

toccata::MidiDisplay::MidiDisplay() {
    m_engine = nullptr;
    m_position = ysMath::Constants::Zero;
    m_size = ysMath::Constants::Zero;

    m_keyStart = 0;
    m_keyEnd = 0;

    m_timeOffset = 0.0;
    m_timeRange = 10.0;
}

toccata::MidiDisplay::~MidiDisplay() {
    /* void */
}

void toccata::MidiDisplay::Initialize(dbasic::DeltaEngine *engine) {
    m_engine = engine;
}

void toccata::MidiDisplay::Process() {
    /* void */
}

void toccata::MidiDisplay::Render() {
    float width = ysMath::GetX(m_size);
    float height = ysMath::GetY(m_size);

    m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
    m_engine->SetObjectTransform(ysMath::TranslationTransform(m_position));
    m_engine->SetLit(false);
    m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0xFF, 0xFF));
    m_engine->DrawBox(width, height);

    const float channelHeight = height / (m_keyEnd - m_keyStart + 1);
    const float start_y = (ysMath::GetY(m_position) - (height / 2)) + (channelHeight / 2);
    for (int i = m_keyStart; i <= m_keyEnd; ++i) {
        const int j = (i - m_keyStart);
        const ysVector channelPosition = ysMath::LoadVector(ysMath::GetX(m_position), start_y + channelHeight * j);

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(channelPosition));
        m_engine->SetLit(false);

        if (IsAccidental(i)) {
            m_engine->SetBaseColor(ysColor::srgbiToLinear(0x40, 0x40, 0x40));
        }
        else {
            m_engine->SetBaseColor(ysColor::srgbiToLinear(0x50, 0x50, 0x50));
        }

        m_engine->DrawBox(width, channelHeight);
    }

    const float lineStart_y = (ysMath::GetY(m_position) - (height / 2));
    for (int i = m_keyStart; i <= m_keyEnd + 1; ++i) {
        const int j = (i - m_keyStart);
        const ysVector linePosition = ysMath::LoadVector(ysMath::GetX(m_position), (int)(lineStart_y + channelHeight * j));

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(linePosition));
        m_engine->SetLit(false);

        m_engine->SetBaseColor(ysColor::srgbiToLinear(0x20, 0x20, 0x20));

        m_engine->DrawBox(width, 1);
    }

    const float start_x = ysMath::GetX(m_position) - (width / 2);

    const int n_ref = m_referenceSegment->NoteContainer.GetCount();
    for (int i = 0; i < n_ref; ++i) {
        const MusicPoint &point = m_referenceSegment->NoteContainer.GetPoints()[i];

        if (point.Pitch > m_keyEnd || point.Pitch < m_keyStart) continue;
        if (point.Timestamp + point.Length < m_timeOffset) continue;
        if (point.Timestamp > m_timeOffset + m_timeRange) continue;

        const float noteWidth = (point.Length / m_timeRange) * width;
        const float x_offset = ((point.Timestamp - m_timeOffset) / m_timeRange) * width;

        const float y = start_y + channelHeight * (point.Pitch - m_keyStart);
        const float x = start_x + x_offset + noteWidth / 2;

        const ysVector position = ysMath::LoadVector(x, y);

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(position));
        m_engine->SetLit(false);
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0xFF, 0x00));

        m_engine->DrawBox(noteWidth, channelHeight * 0.8f);
    }

    const int n = m_inputSegment->NoteContainer.GetCount();
    for (int i = 0; i < n; ++i) {
        const MusicPoint &point = m_inputSegment->NoteContainer.GetPoints()[i];

        if (point.Pitch > m_keyEnd || point.Pitch < m_keyStart) continue;
        if (point.Timestamp + point.Length < m_timeOffset) continue;
        if (point.Timestamp > m_timeOffset + m_timeRange) continue;

        const float noteWidth = (point.Length / m_timeRange) * width;
        const float x_offset = ((point.Timestamp - m_timeOffset) / m_timeRange) * width;

        const float y = start_y + channelHeight * (point.Pitch - m_keyStart);
        const float x = start_x + x_offset + noteWidth / 2;

        const ysVector position = ysMath::LoadVector(x, y);

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(position));
        m_engine->SetLit(false);
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0x00, 0x00, 0x00));

        m_engine->DrawBox(noteWidth, channelHeight * 0.333f);
    }

    for (MatchedBar &bar : m_bars) {
        const float length = bar.Bar.MatchedBar->GetSegment()->Length;
        const float start = Transform::inv_f(0.0, bar.Bar.s, bar.Bar.t);
        const float end = Transform::inv_f(length, bar.Bar.s, bar.Bar.t);

        if (start >= m_timeOffset && start <= m_timeOffset + m_timeRange) {
            const float x_offset = ((start - m_timeOffset) / m_timeRange) * width;
            const ysVector positionStart = ysMath::LoadVector(x_offset + start_x, ysMath::GetY(m_position));

            m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
            m_engine->SetObjectTransform(ysMath::TranslationTransform(positionStart));
            m_engine->SetLit(false);
            m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0x00, 0x00));

            m_engine->DrawBox(2.0, height);
        }

        if (end >= m_timeOffset && end <= m_timeOffset + m_timeRange) {
            const float x_offset = ((end - m_timeOffset) / m_timeRange) * width;
            const ysVector positionEnd = ysMath::LoadVector(x_offset + start_x, ysMath::GetY(m_position));

            m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
            m_engine->SetObjectTransform(ysMath::TranslationTransform(positionEnd));
            m_engine->SetLit(false);
            m_engine->SetBaseColor(ysColor::srgbiToLinear(0x00, 0x00, 0xFF));

            m_engine->DrawBox(2.0, height);
        }
    }
}

bool toccata::MidiDisplay::IsAccidental(int key) const {
    const int note = (key % 12);

    switch (note) {
    case 0: return false;
    case 1: return true;
    case 2: return false;
    case 3: return true;
    case 4: return false;
    case 5: return false;
    case 6: return true;
    case 7: return false;
    case 8: return true;
    case 9: return false;
    case 10: return true;
    case 11: return false;
    default: return false;
    }
}
