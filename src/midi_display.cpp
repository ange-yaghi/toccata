#include "../include/midi_display.h"

#include "../include/transform.h"

toccata::MidiDisplay::MidiDisplay() {
    m_keyStart = 0;
    m_keyEnd = 0;
}

toccata::MidiDisplay::~MidiDisplay() {
    /* void */
}

void toccata::MidiDisplay::Process() {
    /* void */
}

void toccata::MidiDisplay::Render() {
    const float width = m_timeline->GetWidth();
    const float height = m_height;

    const float channelHeight = height / (m_keyEnd - m_keyStart + 1);
    const float start_y = m_positionY;
    const float start_x = m_timeline->GetPositionX();
    const float middle_x = start_x + width / 2;
    const float middle_y = start_y - height / 2;

    const float lower_y = start_y - height;
    for (int i = m_keyStart; i <= m_keyEnd; ++i) {
        const int j = (i - m_keyStart);
        const ysVector channelPosition = 
            ysMath::LoadVector(middle_x, lower_y + channelHeight * j + channelHeight / 2);

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

    for (int i = m_keyStart; i <= m_keyEnd + 1; ++i) {
        const int j = (i - m_keyStart);
        const ysVector linePosition = 
            ysMath::LoadVector(middle_x, (int)(lower_y + channelHeight * j));

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(linePosition));
        m_engine->SetLit(false);

        m_engine->SetBaseColor(ysColor::srgbiToLinear(0x20, 0x20, 0x20));

        m_engine->DrawBox(width, 1);
    }

    MusicSegment *referenceSegment = m_timeline->GetReferenceSegment();
    const int n_ref = referenceSegment->NoteContainer.GetCount();
    for (int i = 0; i < n_ref; ++i) {
        const MusicPoint &point = referenceSegment->NoteContainer.GetPoints()[i];

        if (point.Pitch > m_keyEnd || point.Pitch < m_keyStart) continue;
        if (!m_timeline->InRange(point.Timestamp, point.Timestamp + point.Length)) continue;

        const float noteStart = m_timeline->GetWorldX(point.Timestamp);
        const float noteEnd = m_timeline->GetWorldX(point.Timestamp + point.Length);
        const float noteWidth = noteEnd - noteStart;

        const float y = lower_y + channelHeight * (point.Pitch - m_keyStart) + channelHeight / 2;
        const float x = noteStart + noteWidth / 2;

        const ysVector position = ysMath::LoadVector(x, y);

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(position));
        m_engine->SetLit(false);
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0xFF, 0x00));

        m_engine->DrawBox(noteWidth, channelHeight * 0.8f);
    }

    MusicSegment *inputSegment = m_timeline->GetInputSegment();
    const int n = inputSegment->NoteContainer.GetCount();
    for (int i = 0; i < n; ++i) {
        const MusicPoint &point = inputSegment->NoteContainer.GetPoints()[i];

        if (point.Pitch > m_keyEnd || point.Pitch < m_keyStart) continue;
        if (!m_timeline->InRange(point.Timestamp, point.Timestamp + point.Length)) continue;

        const float noteStart = m_timeline->GetWorldX(point.Timestamp);
        const float noteEnd = m_timeline->GetWorldX(point.Timestamp + point.Length);
        const float noteWidth = noteEnd - noteStart;

        const float y = lower_y + channelHeight * (point.Pitch - m_keyStart) + channelHeight / 2;
        const float x = noteStart + noteWidth / 2;

        const ysVector position = ysMath::LoadVector(x, y);

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(position));
        m_engine->SetLit(false);
        m_engine->SetBaseColor(ysColor::srgbiToLinear(0x00, 0x00, 0x00));

        m_engine->DrawBox(noteWidth, channelHeight * 0.333f);
    }

    const int barCount = m_timeline->GetBarCount();
    for (int i = 0; i < barCount; ++i) {
        const Timeline::MatchedBar &bar = m_timeline->GetBar(i);

        const float length = bar.Bar.MatchedBar->GetSegment()->Length;
        const float start = Transform::inv_f(0.0, bar.Bar.s, bar.Bar.t);
        const float end = Transform::inv_f(length, bar.Bar.s, bar.Bar.t);

        if (m_timeline->InRange(start)) {
            const float x = m_timeline->GetWorldX(start);
            const ysVector positionStart = ysMath::LoadVector(x, middle_y);

            m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
            m_engine->SetObjectTransform(ysMath::TranslationTransform(positionStart));
            m_engine->SetLit(false);
            m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0x00, 0x00));

            m_engine->DrawBox(2.0, height);
        }

        if (m_timeline->InRange(end)) {
            const float x = m_timeline->GetWorldX(end);
            const ysVector positionEnd = ysMath::LoadVector(x, middle_y);

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
