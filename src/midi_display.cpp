#include "../include/midi_display.h"

#include "../include/transform.h"
#include "../include/midi_handler.h"

toccata::MidiDisplay::MidiDisplay() {
    m_keyStart = 0;
    m_keyEnd = 0;

    m_showPlayedNotes = true;
    m_showReferenceNotes = true;

    m_targetVelocity = 50.0;
    m_velocityErrorThreshold = 10.0;

    m_timingErrorThreshold = 0.1;

    m_mode = PracticeMode::Velocity;
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
            ysMath::LoadVector(middle_x, lower_y + channelHeight * j);

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(linePosition));
        m_engine->SetLit(false);

        m_engine->SetBaseColor(ysColor::srgbiToLinear(0x20, 0x20, 0x20));

        m_engine->DrawBox(width, 1);
    }

    if (m_showReferenceNotes) RenderReferenceNotes();
    if (m_showPlayedNotes) RenderPlayedNotes();

    const int barCount = m_timeline->GetBarCount();
    for (int i = 0; i < barCount; ++i) {
        const Timeline::MatchedBar &bar = m_timeline->GetBar(i);

        const double length = bar.Bar.MatchedBar->GetSegment()->GetNormalizedLength();
        const double start = m_timeline->ReferenceToInputSpace(0.0, bar.Bar.T);
        const double end = m_timeline->ReferenceToInputSpace(length, bar.Bar.T);

        if (m_timeline->InRangeInputSpace(start)) {
            const float x = (float)m_timeline->InputSpaceToWorldX(start);
            const ysVector positionStart = ysMath::LoadVector(x + 1.0, middle_y);

            m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
            m_engine->SetObjectTransform(ysMath::TranslationTransform(positionStart));
            m_engine->SetLit(false);
            m_engine->SetBaseColor(ysColor::srgbiToLinear(0xFF, 0x00, 0x00));

            m_engine->DrawBox(2.0, height);
        }

        if (m_timeline->InRangeInputSpace(end)) {
            const float x = (float)m_timeline->InputSpaceToWorldX(end);
            const ysVector positionEnd = ysMath::LoadVector(x - 1.0, middle_y);

            m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
            m_engine->SetObjectTransform(ysMath::TranslationTransform(positionEnd));
            m_engine->SetLit(false);
            m_engine->SetBaseColor(ysColor::srgbiToLinear(0x00, 0x00, 0xFF));

            m_engine->DrawBox(2.0, height);
        }
    }

    RenderCursor();
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

void toccata::MidiDisplay::FindUnmappedNotes(std::set<int> &mapped) const {
    const int n = m_analyzer->GetBarCount();
    for (int i = 0; i < n; ++i) {
        const Analyzer::BarInformation &info = m_analyzer->GetBar(i);
        const Timeline::MatchedBar &bar = m_timeline->GetBar(info.Bar);

        MusicSegment *segment = bar.Bar.MatchedBar->GetSegment();
        const int n_ref = segment->NoteContainer.GetCount();
        for (int j = 0; j < n_ref; ++j) {
            const Analyzer::NoteInformation &noteInfo = info.NoteInformation[j];
            if (noteInfo.InputNote != -1) {
                mapped.emplace(noteInfo.InputNote);
            }
        }
    }
}

ysVector toccata::MidiDisplay::GetColor(const Analyzer::NoteInformation &noteInfo) const {
    if (m_mode == PracticeMode::Default) {
        if (noteInfo.InputNote == -1) {
            return ysColor::srgbiToLinear(0xFF, 0x00, 0x00);
        }
        else {
            return ysColor::srgbiToLinear(0xFF, 0xFF, 0x00);
        }
    }
    else if (m_mode == PracticeMode::Timing) {
        if (noteInfo.InputNote == -1) {
            return ysColor::srgbiToLinear(0x44, 0x44, 0x44);
        }
        else {
            return GetTimingColor(noteInfo.Error);
        }
    }
    else if (m_mode == PracticeMode::Velocity) {
        if (noteInfo.InputNote == -1) {
            return ysColor::srgbiToLinear(0x44, 0x44, 0x44);
        }
        else {
            const unsigned short velocity =
                m_timeline
                ->GetInputSegment()
                ->NoteContainer.GetPoints()[noteInfo.InputNote]
                .Velocity;
            return GetVelocityColor(velocity);
        }
    }

    return ysMath::Constants::Zero3;
}

ysVector toccata::MidiDisplay::GetVelocityColor(unsigned short velocity) const {
    const int error = (int)velocity - m_targetVelocity;
    const double s = error / m_velocityErrorThreshold;

    const ysVector color = m_settings->BarDisplay_VelocityHeatMap->Sample(s);
    return color;
}

ysVector toccata::MidiDisplay::GetTimingColor(double error) const {
    const double s = error / m_timingErrorThreshold;

    const ysVector color = m_settings->BarDisplay_TimingHeatMap->Sample(s);
    return color;
}

void toccata::MidiDisplay::RenderReferenceNotes() {
    const float width = m_timeline->GetWidth();
    const float height = m_height;

    const float channelHeight = height / (m_keyEnd - m_keyStart + 1);
    const float start_y = m_positionY;
    const float start_x = m_timeline->GetPositionX();
    const float middle_x = start_x + width / 2;
    const float middle_y = start_y - height / 2;

    const float lower_y = start_y - height;

    const int n = m_analyzer->GetBarCount();
    for (int i = 0; i < n; ++i) {
        const Analyzer::BarInformation &info = m_analyzer->GetBar(i);
        const Timeline::MatchedBar &bar = m_timeline->GetBar(info.Bar);

        MusicSegment *segment = bar.Bar.MatchedBar->GetSegment();
        const int n_ref = segment->NoteContainer.GetCount();
        for (int j = 0; j < n_ref; ++j) {
            const Analyzer::NoteInformation &noteInfo = info.NoteInformation[j];
            const MusicPoint &point = segment->NoteContainer.GetPoints()[j];

            if (point.Pitch > m_keyEnd || point.Pitch < m_keyStart) continue;

            const double start_norm = segment->Normalize(point.Timestamp);
            const double end_norm = segment->Normalize(point.GetEnd());

            const double start_inputSpace = m_timeline->ReferenceToInputSpace(start_norm, bar.Bar.T);
            const double end_inputSpace = m_timeline->ReferenceToInputSpace(end_norm, bar.Bar.T);

            if (!m_timeline->InRangeInputSpace(start_inputSpace, end_inputSpace)) continue;

            const double noteStart = m_timeline->InputSpaceToWorldX(start_inputSpace);
            const double noteEnd = m_timeline->InputSpaceToWorldX(end_inputSpace);
            const double noteWidth = noteEnd - noteStart;

            const float y = lower_y + channelHeight * (point.Pitch - m_keyStart) + channelHeight / 2;
            const float x = (float)(noteStart + noteWidth / 2);

            const ysVector position = ysMath::LoadVector(x, y);

            m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
            m_engine->SetObjectTransform(ysMath::TranslationTransform(position));
            m_engine->SetLit(false);

            m_engine->SetBaseColor(GetColor(noteInfo));

            m_engine->DrawBox((float)noteWidth, channelHeight * 0.8f);
        }
    }
}

void toccata::MidiDisplay::RenderPlayedNotes() {
    const float height = m_height;

    const float channelHeight = height / (m_keyEnd - m_keyStart + 1);
    const float start_y = m_positionY;
    const float middle_y = start_y - height / 2;

    const float lower_y = start_y - height;

    std::set<int> mappedNotes;
    FindUnmappedNotes(mappedNotes);
    MusicSegment *inputSegment = m_timeline->GetInputSegment();
    const int n = inputSegment->NoteContainer.GetCount();
    for (int i = 0; i < n; ++i) {
        const MusicPoint &point = inputSegment->NoteContainer.GetPoints()[i];

        if (point.Pitch > m_keyEnd || point.Pitch < m_keyStart) continue;
        if (!m_timeline->InRange(point.Timestamp, point.Timestamp + point.Length)) continue;

        const double noteStart = m_timeline->TimestampToWorldX(point.Timestamp);
        const double noteEnd = m_timeline->TimestampToWorldX(point.Timestamp + point.Length);
        const double noteWidth = noteEnd - noteStart;

        const float y = (float)(lower_y + channelHeight * (point.Pitch - m_keyStart) + channelHeight / 2);
        const float x = (float)(noteStart + noteWidth / 2);

        const ysVector position = ysMath::LoadVector(x, y);

        m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
        m_engine->SetObjectTransform(ysMath::TranslationTransform(position));
        m_engine->SetLit(false);

        if (m_showReferenceNotes) {
            ysVector color;
            if (mappedNotes.count(i) == 0) {
                color = ysColor::srgbiToLinear(0xFF, 0x00, 0x00);
            }
            else {
                color = ysColor::srgbiToLinear(0x00, 0x00, 0x00);
            }

            m_engine->SetBaseColor(color);
            m_engine->DrawBox((float)noteWidth, channelHeight * 0.333f);
        }
        else {
            ysVector color;
            if (m_mode == PracticeMode::Timing) {
                color = ysColor::srgbiToLinear(0x00, 0x00, 0x00);
            }
            else if (m_mode == PracticeMode::Velocity) {
                color = GetVelocityColor(point.Velocity);
            }
            else if (m_mode == PracticeMode::Default) {
                color = ysColor::srgbiToLinear(0x00, 0x00, 0x00);
            }

            m_engine->SetBaseColor(color);
            m_engine->DrawBox((float)noteWidth, channelHeight);
        }
    }
}

void toccata::MidiDisplay::RenderCursor() {
    const float height = m_height;

    const float channelHeight = height / (m_keyEnd - m_keyStart + 1);
    const float start_y = m_positionY;
    const float middle_y = start_y - height / 2;

    const timestamp currentTimestamp = MidiHandler::Get()->GetEstimatedTimestamp();
    const double cursor = m_timeline->TimestampToInputSpace(currentTimestamp);

    const float x = (float)m_timeline->InputSpaceToWorldX(cursor);
    const ysVector positionEnd = ysMath::LoadVector(x - 1.0, middle_y);

    m_engine->SetDrawTarget(dbasic::DeltaEngine::DrawTarget::Gui);
    m_engine->SetObjectTransform(ysMath::TranslationTransform(positionEnd));
    m_engine->SetLit(false);
    m_engine->SetBaseColor(ysColor::srgbiToLinear(0x00, 0x00, 0x00));

    m_engine->DrawBox(2.0, height);
}
