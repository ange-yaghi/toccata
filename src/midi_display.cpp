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

    m_timingErrorMin = 0.1;
    m_timingErrorMax = 0.5;

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

        const ysVector color = IsAccidental(i)
            ? m_settings->MidiDisplay_AccidentalChannelColor
            : m_settings->MidiDisplay_NaturalChannelColor;

        DrawBox(BoundingBox(width, channelHeight)
            .AlignCenterY(lower_y + channelHeight * j + channelHeight / 2)
            .AlignLeft(start_x), color);
    }

    for (int i = m_keyStart; i <= m_keyEnd + 1; ++i) {
        const int j = (i - m_keyStart);

        const ysVector color = m_settings->MidiDisplay_ChannelLineColor;
        DrawBox(BoundingBox(width, 1)
            .AlignCenterY(lower_y + channelHeight * j)
            .AlignLeft(start_x), color);
    }

    if (m_showReferenceNotes) RenderReferenceNotes();
    if (m_showPlayedNotes) {
        RenderPlayedNotes();
        RenderUnresolvedNotes();
    }

    const int barCount = m_timeline->GetBarCount();
    for (int i = 0; i < barCount; ++i) {
        const Timeline::MatchedBar &bar = m_timeline->GetBar(i);

        const double length = bar.Bar.MatchedBar->GetSegment()->GetNormalizedLength();
        const double start = m_timeline->ReferenceToInputSpace(0.0, bar.Bar.T);
        const double end = m_timeline->ReferenceToInputSpace(length, bar.Bar.T);

        if (m_timeline->InRangeInputSpace(start)) {
            const float x = (float)m_timeline->InputSpaceToWorldX(start);

            const ysVector color = m_settings->MidiDisplay_BarStartLineColor;
            DrawBox(BoundingBox(2.0, height)
                .AlignLeft(x)
                .AlignTop(start_y), color);
        }

        if (m_timeline->InRangeInputSpace(end)) {
            const float x = (float)m_timeline->InputSpaceToWorldX(end);

            const ysVector color = m_settings->MidiDisplay_BarEndLineColor;
            DrawBox(BoundingBox(2.0, height)
                .AlignRight(x)
                .AlignTop(start_y), color);
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
        const Timeline::MatchedBar &bar = m_timeline->GetBar(info.MasterIndex);

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
            return m_settings->MidiDisplay_MissedNoteColor;
        }
        else {
            return m_settings->MidiDisplay_DefaultNoteColor;
        }
    }
    else if (m_mode == PracticeMode::Timing) {
        if (noteInfo.InputNote == -1) {
            return m_settings->MidiDisplay_IgnoredNoteColor;
        }
        else {
            return GetTimingColor(noteInfo.Error);
        }
    }
    else if (m_mode == PracticeMode::Velocity) {
        if (noteInfo.InputNote == -1) {
            return m_settings->MidiDisplay_IgnoredNoteColor;
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

    const ysVector color = m_settings->MidiDisplay_VelocityHeatMap->Sample(s);
    return color;
}

ysVector toccata::MidiDisplay::GetTimingColor(double error) const {
    const double s = (error - m_timingErrorMin) / (m_timingErrorMax - m_timingErrorMin);

    const ysVector color = m_settings->MidiDisplay_TimingHeatMap->Sample(s);
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
        const Timeline::MatchedBar &bar = m_timeline->GetBar(info.MasterIndex);

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

            const ysVector color = GetColor(noteInfo);
            DrawBox(BoundingBox(noteWidth, channelHeight)
                .AlignLeft(noteStart)
                .AlignCenterY(y), color);
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

        if (m_showReferenceNotes) {
            ysVector color;
            if (mappedNotes.count(i) == 0) {
                color = m_settings->MidiDisplay_UnmappedPlayedNoteColor;
            }
            else {
                color = m_settings->MidiDisplay_MappedPlayedNoteColor;
            }

            DrawBox(BoundingBox(noteWidth, channelHeight * 0.333f)
                .AlignCenterY(y)
                .AlignLeft(noteStart), color);
        }
        else {
            ysVector color = ysMath::Constants::Zero3;
            if (m_mode == PracticeMode::Timing) {
                color = m_settings->MidiDisplay_DefaultPlayedNoteColor;
            }
            else if (m_mode == PracticeMode::Velocity) {
                color = GetVelocityColor(point.Velocity);
            }
            else if (m_mode == PracticeMode::Default) {
                color = m_settings->MidiDisplay_DefaultPlayedNoteColor;
            }

            DrawBox(BoundingBox(noteWidth, channelHeight)
                .AlignCenterY(y)
                .AlignLeft(noteStart), color);
        }
    }
}

void toccata::MidiDisplay::RenderUnresolvedNotes() {
    const float height = m_height;

    const float channelHeight = height / (m_keyEnd - m_keyStart + 1);
    const float start_y = m_positionY;
    const float middle_y = start_y - height / 2;

    const float lower_y = start_y - height;

    const timestamp currentTimestamp = MidiHandler::Get()->GetEstimatedTimestamp();

    MusicSegment *unfinishedSegment = m_timeline->GetUnfinishedSegment();
    const int n = unfinishedSegment->NoteContainer.GetCount();
    for (int i = 0; i < n; ++i) {
        const MusicPoint &point = unfinishedSegment->NoteContainer.GetPoints()[i];

        if (point.Pitch > m_keyEnd || point.Pitch < m_keyStart) continue;
        if (!m_timeline->InRange(point.Timestamp, currentTimestamp)) continue;

        const double noteStart = m_timeline->TimestampToWorldX(point.Timestamp);
        const double noteEnd = m_timeline->TimestampToWorldX(currentTimestamp);
        const double noteWidth = noteEnd - noteStart;

        const float y = (float)(lower_y + channelHeight * (point.Pitch - m_keyStart) + channelHeight / 2);

        DrawBox(BoundingBox(noteWidth, channelHeight)
            .AlignCenterY(y)
            .AlignLeft(noteStart), ysMath::Constants::One);
    }
}

void toccata::MidiDisplay::RenderCursor() {
    const float height = m_height;

    const float channelHeight = height / (m_keyEnd - m_keyStart + 1);

    const timestamp currentTimestamp = MidiHandler::Get()->GetEstimatedTimestamp();
    const double cursor = m_timeline->TimestampToInputSpace(currentTimestamp);

    const float x = (float)m_timeline->InputSpaceToWorldX(cursor);

    const ysVector color = m_settings->MidiDisplay_CursorColor;

    DrawBox(BoundingBox(2.0, height)
        .AlignCenterX(x)
        .AlignTop(m_positionY), color);
}
